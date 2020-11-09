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

#include "Event.h"
#include "Pin.h"
#include "CodalFiber.h"

#ifndef CODAL_PULSE_IN_H
#define CODAL_PULSE_IN_H

#define DEVICE_PIN_EVT_PULSE_TIMEOUT           1000

namespace codal
{

class PulseIn
{
    Pin             &pin;
    uint32_t        lastPeriod;
    FiberLock       lock;
    CODAL_TIMESTAMP timeout;   
    static bool     timeoutGeneratorStarted;

    public:
    /**
     * Creates a new instance of a synchronous pulse detector ont he given pin.
     * 
     * @param pin The pin to observe for pulse events
     * @return the period of the next pulse in microseconds, or XXX if the given timeout expires.
     */
    PulseIn(Pin &pin);

    /**
     * Synchronously await a pulse, and return the period of the pulse.
     * 
     * @param timeout The maximum amount of time to wait for a pulse, in microseconds. Set to zero to wait indefinitely.
     * @return The period of the next pulse, in microseconds, or DEVICE_CANCELLED if the timeout passes.
     */
    int awaitPulse(int timeout = 0);

    /**
     * Event handler called when a pulse is detected.
     */
    void
    onPulse(Event e);

    /**
     * Event handler called when a timeout event is generated.
     */
    void
    onTimeout(Event e);

    /**
     * Destructor
     */
    ~PulseIn();
};

}
#endif
