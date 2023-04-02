/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.

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

#ifndef DATALOG_MAILBOX_HANDLER_H
#define DATALOG_MAILBOX_HANDLER_H

#include "JacdacMailbox.h"
#include "MicroBitLog.h"

// Known service ID numbers for micro:bit mailbox services
#define DATALOG_MAILBOX_COMMAND_REQUEST_COLUMN_NAMES      1         // TODO
#define DATALOG_MAILBOX_COMMAND_GET_CSV_DATA              2
#define DATALOG_MAILBOX_COMMAND_SET_STREAMING_MODE        3         // TODO

#define DATALOG_MAILBOX_EVENT_COMMAND_READY               1

namespace codal
{
    class DataLogMailboxHandler : JacdacMailboxHandler
    {
        jd_packet_t request;
        MicroBitLog &log;

        public:
        /**
        * Constructor.
        * Create a DataLogMailboxHandler instance, and register it with the given mailbox.
        *
        * @param log The MicroBitLog to utilise.
        * @param m the Jacdac mailbox to use. If NULL, the singleton mailbox is chosen.
        */
        DataLogMailboxHandler(MicroBitLog &l, JacdacMailbox *m = NULL);

        /**
         * Process a frame that has been received by the mailbox 
         * This method will be invokedfor on each packet received. 
         * NOTE: This may (likely) be called in interrupt context, with interrupts disabled,
         * so decoupling via the scheduler is highly recommended for complex operations
         *
         * @param frame The data that has been received
         */
        virtual void process(const jd_frame_t *frame) override;

        /**
         * Callback method, invoked as a deferred procedure call from process()
         */
        void onCommandRequest(Event e);
    };
}

#endif