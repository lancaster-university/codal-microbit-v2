#ifndef InterruptIn_h
#define InterruptIn_h

#include "mbed.h"

class InterruptIn {


    public:
        MessageBus bus;
        NRF52Pin p;

        InterruptIn(PinName pin) : p(DEVICE_ID_IO_MBED_INTERRUPT_IN, pin, PIN_CAPABILITY_ANALOG) {
            // p.enableRiseFallEvents(IO_STATUS_INTERRUPT_ON_EDGE);
            return;
        }

        void mode(PullMode pull) {
            p.setPull(pull);
        }

        template<typename T>
        void fall(T* tptr, void (T::*mptr)(void)) {
            // p.
        }

        template<typename T>
        void rise(T* tptr, void (T::*mptr)(void)) {
            // p.
        }
    
};

#endif
