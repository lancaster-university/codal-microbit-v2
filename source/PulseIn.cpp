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

#include "PulseIn.h"
#include "Timer.h"
#include "CodalDmesg.h"

using namespace codal;

bool PulseIn::timeoutGeneratorStarted = false;

/**
 * Creates a new instance of a synchronous pulse detector ont he given pin.
 * 
 * @param pin The pin to observe for pulse events
 * @return the period of the next pulse in microseconds, or XXX if the given timeout expires.
 */
PulseIn::PulseIn(Pin &p) : pin(p)
{
    lastPeriod = 0;

    // Configure the requested pin to supply pulse events.
    pin.eventOn(DEVICE_PIN_EVENT_ON_PULSE);
    EventModel::defaultEventBus->listen(pin.id, pin.getPolarity() ? DEVICE_PIN_EVT_PULSE_HI : DEVICE_PIN_EVT_PULSE_LO, this, &PulseIn::onPulse);
    EventModel::defaultEventBus->listen(pin.id, DEVICE_PIN_EVT_PULSE_TIMEOUT, this, &PulseIn::onTimeout);
    
    if (!timeoutGeneratorStarted)
    {
        system_timer_event_every_us(10000, pin.id, DEVICE_PIN_EVT_PULSE_TIMEOUT);
        timeoutGeneratorStarted = true;
    }

    lock.wait();
}

/**
 * Synchronously await a pulse, and return the period of the pulse.
 * 
 * @param timeout The maximum amount of time to wait for a pulse, in microseconds. Set to zero to wait indefinitely.
 * @return The period of the next pulse, in microseconds, or DEVICE_CANCELLED if the timeout passes.
 */
int 
PulseIn::awaitPulse(int timeout)
{
    if (timeout)
        this->timeout = system_timer_current_time_us() + timeout;
    else
        this->timeout = 0;
    
    lastPeriod = 0;

    lock.wait();

    if (lastPeriod != 0)
        return lastPeriod;
    else
        return DEVICE_CANCELLED;   
}

/**
 * Event handler called when a pulse is detected.
 */
void
PulseIn::onPulse(Event e)
{
    timeout = 0;
    lastPeriod = (uint32_t) e.timestamp;
    
    // Wake any blocked fibers and reset the lock.
    if (lock.getWaitCount())
    {
        lock.notify();
        lock.wait();
    }
}

/**
 * Event handler called when a timeout event is generated.
 */
void
PulseIn::onTimeout(Event e)
{
    if (timeout && system_timer_current_time_us() > timeout)
    {
        timeout = 0;
        lastPeriod = 0;

        if (lock.getWaitCount())
        {
            lock.notify();
            lock.wait();
        }
    }
}

/**
 * Destructor
 */
PulseIn::~PulseIn()
{
    EventModel::defaultEventBus->ignore(pin.id, pin.getPolarity() ? DEVICE_PIN_EVT_PULSE_HI : DEVICE_PIN_EVT_PULSE_LO, this, &PulseIn::onPulse);
    EventModel::defaultEventBus->ignore(pin.id, DEVICE_PIN_EVT_PULSE_TIMEOUT, this, &PulseIn::onTimeout);
    pin.eventOn(DEVICE_PIN_EVENT_NONE);
    timeout = 0;
    lastPeriod = 0;
    lock.notifyAll();
}