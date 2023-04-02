/*
The MIT License (MIT)

Copyright (c) 2020 Lancaster University.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "JacdacMailbox.h"
#include "CodalDmesg.h"

using namespace codal;

/**
 * Initialize singleton
 */
JacdacMailbox* JacdacMailbox::instance = NULL;

/**
 * Constructor.
 * Create a JacdacMailbox instance.
 *
 * @param memoryRegion Pointer to a RAM data structure where the address of the mailbox in memory can be registered.
 */
JacdacMailbox::JacdacMailbox(MicroBitNoInitMemoryRegion *memoryRegion)
{
    // Allocate memory for the shared memory mailbox.
    exchangeBuffer = new JacdacMailboxBuffer;
    memset(exchangeBuffer, 0, sizeof(*exchangeBuffer));

    // Initialize the list of services to empty.
    memset(services, 0, sizeof(*services));

    // Initialize mailbox magic and mark the buffer to indicate we are ready for communication to begin.
    memcpy(exchangeBuffer->magic, "JDmx\xe9\xc0\xa6\xb0", 8);
    exchangeBuffer->outputBuffer[2] = 0xff;

    // Store the address of the exchange buffer in part of the NOINIT region (a well known location)
    memoryRegion->mailboxBaseAddress = (uint32_t) exchangeBuffer;

    // Store the IRQ for the PC to use to indicate new data is available
    exchangeBuffer->irqn = TEMP_IRQn;

    // Record this object instance as the class singleton, if it is the first to be allocated.
    if (JacdacMailbox::instance == NULL)
        JacdacMailbox::instance = this;

    // Initialize interrupt callback
    NVIC_ClearPendingIRQ((IRQn_Type)exchangeBuffer->irqn);
    NVIC_EnableIRQ((IRQn_Type)exchangeBuffer->irqn);
}

/**
 * Add a protocol service handler. 
 * All data handlers are invoked on each packet received. 
 * NOTE: This may (likely) be called in nterrupt context, so decoupling via scheduler is recommended for complex operations
 *
 * @param handler A class implementing the JacdacMailboxHandler to add
 * @return DEVICE_OK on success
 */
int JacdacMailbox::addHandler(JacdacMailboxHandler *handler)
{
    for (int i=0; i<JACDAC_MAILBOX_MAXIMUM_SERVICES; i++)
    {
        if (services[i] == NULL)
        {
            services[i] = handler;
            return DEVICE_OK;
        }
    }

    return DEVICE_NO_RESOURCES;
}

/**
 * Send a given buffer to an attached PC via the mailbox. 
 * If the mailbox is empty, it is scheduled for immediate transmission. if not, it is queued awaiting collection.
 * @param frame the jacdac frame to send
 * @return DEVICE_OK, or DEVICE_NO_RESOURCES if the outputQueue is full.
 */
int JacdacMailbox::sendFrame(const jd_frame_t *frame)
{
    target_disable_irq();

    if (exchangeBuffer->outputBuffer[2] != 0xff) {
        // If the output buffer is already full, queue the frame.
        // Otherwise, copy it directly into the output buffer.

        if (outputQueue || getOutputPointer() == NULL)
            queueOutputFrame(frame);
        else
            writeOutputFrame(frame);
    }

    target_enable_irq();

    processQueues();

    return DEVICE_OK;
}

/**
 * Attempts to flush the input and output buffers.
 * A succesfully received buffer is delivered to all registered handers.
 * Schedule the next packet to be sent from the outputQueue is applicable.
 */
void JacdacMailbox::processQueues()
{
    target_disable_irq();

    // If we have a packet on the output queue and the outputbuffer is empty, send it.
    if (outputQueue && getOutputPointer()) {
        auto tmp = outputQueue;
        outputQueue = tmp->next;
        sendFrame(&tmp->frame);
        free(tmp);
    }

    // If we have a packet on the input (from the PC), forward it on to all handlers
    if (exchangeBuffer->inputBuffer[2])
    {
        for (int i=0; i<JACDAC_MAILBOX_MAXIMUM_SERVICES; i++)
        {
            if (services[i] != NULL)
                services[i]->process((const jd_frame_t *)exchangeBuffer->inputBuffer);
        }
        exchangeBuffer->inputBuffer[2] = 0;
    }

    target_enable_irq();
}

/**
 * Provides a pointer to the outputbuffer if available, NULL otherwise.
 * @return a pointer to the start of the output buffer if the buffer is free, and NULL otherwise.
 */
volatile uint32_t* JacdacMailbox::getOutputPointer()
{
    volatile uint8_t* bp = exchangeBuffer->outputBuffer;

    if (bp[2])
        bp = NULL;

    return (volatile uint32_t *) bp;
}

/**
 * Queue outputBuffer to transmission to attached PC.
 * @param frame The Jacdac frame to send - the frame is duplicated once queued.
 * @return DEVICE_OK
 */
int JacdacMailbox::writeOutputFrame(const jd_frame_t *frame)
{
    // Get reference to output buffer if possible.
    auto bp = getOutputPointer();

    // This should never be NULL, unless we have a race condition somewhere. Assert a panic if necessary.
    if (bp == NULL)
        target_panic(111);

    // Copy the packet data in byte by byte, leaving the first word to last to ensure atomicity.
    auto src = (uint32_t *)frame;
    int len = (JD_FRAME_SIZE(frame) + 3) >> 2;
    for (int i = 1; i < len; ++i)
        bp[i] = src[i];

    bp[0] = src[0];

    return DEVICE_OK;
}

/**
 * Queue outputBuffer to transmission to attached PC.
 * @param frame The Jacdac frame to send - the frame is duplicated once queued.
 * @return DEVICE_OK, or DEVICE_NO_RESOURCES if the outputQueue is full.
 */
int JacdacMailbox::queueOutputFrame(const jd_frame_t *frame)
{
    auto buf = (LinkedFrame *)malloc(JD_FRAME_SIZE(frame) + JACDAC_MAILBOX_LINKED_FRAME_HEADER_SIZE);

    buf->timestamp_ms = (uint32_t) system_timer_current_time();
    memcpy(&buf->frame, frame, JD_FRAME_SIZE(frame));

    target_disable_irq();

    auto last = outputQueue;
    int num = 0;
    buf->next = NULL;

    while (last && last->next) {
        last = last->next;
        num++;
    }

    if (num < JACDAC_MAILBOX_MAXIMUM_QUEUE_SIZE) {
        if (last)
            last->next = buf;
        else
            outputQueue = buf;

        buf = NULL;
    }

    target_enable_irq();

    if (buf == NULL) {
        return DEVICE_OK;
    } else {
        free(buf);
        return DEVICE_NO_RESOURCES;
    }
}

extern "C" void TEMP_IRQHandler() {
    if (JacdacMailbox::instance != NULL)
        JacdacMailbox::instance->processQueues();
}