#ifndef InterruptIn_h
#define InterruptIn_h

#include "Pin.h"
#include "MbedMemberFunctionCallback.h"
#include "MicroBitEvent.h"

#define DEVICE_ID_MBED_INTERRUPT_IN 0xE0

extern MicroBit uBit;

class InterruptIn {
    
    MbedMemberFunctionCallback *_rise;
    MbedMemberFunctionCallback *_fall;
    NRF52Pin p;

    public:
        InterruptIn(PinName pin) : p(DEVICE_ID_MBED_INTERRUPT_IN, pin, PIN_CAPABILITY_DIGITAL) {
            p.eventOn(DEVICE_PIN_INTERRUPT_ON_EDGE);
            return;
        }

        void mode(int pull) {
            switch(pull) {
                case PullNone:
                    p.setPull(PullMode::None);
                    break;
                case PullDown:
                    p.setPull(PullMode::Down);
                    break;
                case PullUp:
                    p.setPull(PullMode::Up);
                    break;
                default:
                    p.setPull(PullMode::Up);
                    break;
            }
        }

        void onFall(MicroBitEvent e) {
            _fall->fire();
        }

        template<typename T>
        void fall(T* tptr, void (T::*mptr)(void)) {
            this->_fall = new MbedMemberFunctionCallback(tptr, mptr);
            uBit.messageBus.listen(DEVICE_ID_MBED_INTERRUPT_IN, DEVICE_PIN_EVT_FALL, this, &InterruptIn::onFall);
        }

        void onRise(MicroBitEvent e) {
            _rise->fire();
        }

        template<typename T>
        void rise(T* tptr, void (T::*mptr)(void)) {
            this->_rise = new MbedMemberFunctionCallback(tptr, mptr);
            uBit.messageBus.listen(DEVICE_ID_MBED_INTERRUPT_IN, DEVICE_PIN_EVT_RISE, this, &InterruptIn::onRise);
        }
    
};

#endif
