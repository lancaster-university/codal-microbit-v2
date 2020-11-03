#ifndef InterruptIn_h
#define InterruptIn_h

#include "mbed.h"
#include "Pin.h"
#include "MemberFunctionCallback.h"

class InterruptIn {
    
    MemberFunctionCallback *_rise;
    MemberFunctionCallback *_fall;

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

        void onFall() {
            MicroBitEvent e;
            _fall->fire(e);
        }

        template<typename T>
        void fall(T* tptr, void (T::*mptr)(void)) {
            this->_fall = new MemberFunctionCallback(tptr, mptr);
            bus.listen(DEVICE_ID_IO_MBED_INTERRUPT_IN, DEVICE_PIN_EVT_FALL, this, &InterruptIn::onFall);
        }

        void onRise() {
            MicroBitEvent e;
            _rise->fire(e);
        }

        template<typename T>
        void rise(T* tptr, void (T::*mptr)(void)) {
            this->_rise = new MemberFunctionCallback(tptr, mptr);
            bus.listen(DEVICE_ID_IO_MBED_INTERRUPT_IN, DEVICE_PIN_EVT_RISE, this, &InterruptIn::onRise);
        }
    
};

#endif
