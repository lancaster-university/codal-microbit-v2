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
 * Constructor.
 * Create a JacdacMailbox instance.
 *
 * @param memoryRegion Pointer to a RAM data structure where the address of the mailbox in memory can be registered.
 */
JacdacMailbox::JacdacMailbox(MicroBitNoInitMemoryRegion *memoryRegion)
{
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
    return DEVICE_OK;
}

/**
 * Send a given buffer to an attached PC via the mailbox. 
 * If the mailbox is empty, it is scheduled for immediate transmission. if not, it is queued awaiting collection.
 * @param frame the jacdac frame to send
 * @return DEVICE_OK, or DEVICE_INSUFFICIENT_RESOURCES if the outputQueue is full.
 */
int JacdacMailbox::sendFrame(const jd_frame_t *frame)
{
    return DEVICE_OK;
}

/**
 * Attempts to flush the input and output buffers.
 * A succesfully received buffer is delivered to all registered handers.
 * Schedule the next packet to be sent from the outputQueue is applicable.
 */
void JacdacMailbox::processQueues()
{
}