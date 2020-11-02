#ifndef InterruptIn_h
#define InterruptIn_h

#include "mbed.h"
#include "Pin.h"

class InterruptIn {


    public:
        MessageBus bus;
        NRF52Pin p;

        InterruptIn(PinName pin) : p(DEVICE_ID_IO_MBED_INTERRUPT_IN, pin, PIN_CAPABILITY_ANALOG) {
            p.eventOn(DEVICE_PIN_INTERRUPT_ON_EDGE);
            return;
        }

        void mode(PullMode pull) {
            p.setPull(pull);
        }

        template<typename T>
        void fall(T* tptr, void (T::*mptr)(void)) {
            bus.listen(DEVICE_ID_IO_MBED_INTERRUPT_IN, DEVICE_PIN_EVT_FALL, (&T)::*mptr, MESSAGE_BUS_LISTENER_IMMEDIATE);
        }

        template<typename T>
        void rise(T* tptr, void (T::*mptr)(void)) {
            bus.listen(DEVICE_ID_IO_MBED_INTERRUPT_IN, DEVICE_PIN_EVT_RISE, (&T)::*mptr, MESSAGE_BUS_LISTENER_IMMEDIATE);
        }
    
};

#endif
