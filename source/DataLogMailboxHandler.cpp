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

#include "DataLogMailboxHandler.h"
#include "CodalDmesg.h"

using namespace codal;

/**
 * Constructor.
 * Create a DataLogMailboxHandler instance, and register it with the given mailbox.
 *
 * @param log The MicroBitLog to utilise.
 * @param m the Jacdac mailbox to use. If NULL, the singleton mailbox is chosen.
 */
DataLogMailboxHandler::DataLogMailboxHandler(MicroBitLog &l, JacdacMailbox *m) : JacdacMailboxHandler(m), log(l)
{
    if(EventModel::defaultEventBus)
        EventModel::defaultEventBus->listen(id, DATALOG_MAILBOX_EVENT_COMMAND_READY, this, &DataLogMailboxHandler::onCommandRequest);
}

/**
 * Process a frame that has been received by the mailbox 
 * This method will be invokedfor on each packet received. 
 * NOTE: This may (likely) be called in interrupt context, with interrupts disabled,
 * so decoupling via the scheduler is highly recommended for complex operations
 *
 * @param frame The data that has been received
 */
void DataLogMailboxHandler::process(const jd_frame_t *frame)
{
    jd_packet_t *packet = (jd_packet_t *) frame;

    // Validate that this message is for us. If not, ignore it.
    if (busy || frame == NULL || frame->size == 0 || frame->device_identifier != MICROBIT_MAILBOX_DEVICE_ID || packet->service_number != JACDAC_MAILBOX_SERVICE_ID_DATALOG)
        return;
 
    busy = true;
    request = *packet;
    requestIndex = *((uint32_t *) &packet->data[0]);

    Event(id, DATALOG_MAILBOX_EVENT_COMMAND_READY);
}

/**
 * Callback method, invoked as a deferred procedure call from process()
 */
void DataLogMailboxHandler::onCommandRequest(Event e)
{
    ManagedBuffer b(JD_SERIAL_FULL_HEADER_SIZE + JD_SERIAL_PAYLOAD_SIZE);
    jd_packet_t *response = (jd_packet_t *) &b[0];

    // Copy in the header from the request, and adapt as a response.
    *response = request;
    response->_size = 128 + 4;
    response->flags &= ~ 0x01;

    if (request.service_command == DATALOG_MAILBOX_COMMAND_GET_CSV_DATA)
    {
        uint32_t maxlength = log.getDataLength(DataFormat::CSV);
        log.readData(&b[sizeof(jd_packet_t)], requestIndex, 128, DataFormat::CSV, maxlength);
    }

    mailbox->sendFrame((const jd_frame_t *) response);
    busy = false;
}
