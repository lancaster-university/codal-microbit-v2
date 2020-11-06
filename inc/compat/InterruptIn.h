#ifndef InterruptIn_h
#define InterruptIn_h

/* uBit object from PXT or CODAL
 * TODO: MICROBIT_DAL_PXT will be added to PXT to replace this define */
#ifdef MICROBIT_DAL_FIBER_USER_DATA
#include "pxt.h"
#else
extern MicroBit uBit;
#endif

#include "MicroBitCompat.h"
#include "Pin.h"
#include "MbedMemberFunctionCallback.h"
#include "MicroBitEvent.h"

class InterruptIn {
    
    MbedMemberFunctionCallback *_rise;
    MbedMemberFunctionCallback *_fall;
    NRF52Pin *p;

    public:
        InterruptIn(PinName pin) {
            p = new NRF52Pin(MICROBIT_ID_MBED_INTERRUPT_IN, pin, PIN_CAPABILITY_DIGITAL);
            p->eventOn(MICROBIT_PIN_EVENT_ON_EDGE);
            return;
        }

        void mode(int pull) {
            switch(pull) {
                case PullNone:
                    p->setPull(PullMode::None);
                    break;
                case PullDown:
                    p->setPull(PullMode::Down);
                    break;
                case PullUp:
                    p->setPull(PullMode::Up);
                    break;
                default:
                    p->setPull(PullMode::Up);
                    break;
            }
        }

        void onFall(MicroBitEvent e) {
            _fall->fire();
        }

        template<typename T>
        void fall(T* tptr, void (T::*mptr)(void)) {
            this->_fall = new MbedMemberFunctionCallback(tptr, mptr);
            uBit.messageBus.listen(MICROBIT_ID_MBED_INTERRUPT_IN, MICROBIT_PIN_EVT_FALL, this, &InterruptIn::onFall);
        }

        void onRise(MicroBitEvent e) {
            _rise->fire();
        }

        template<typename T>
        void rise(T* tptr, void (T::*mptr)(void)) {
            this->_rise = new MbedMemberFunctionCallback(tptr, mptr);
            uBit.messageBus.listen(MICROBIT_ID_MBED_INTERRUPT_IN, MICROBIT_PIN_EVT_RISE, this, &InterruptIn::onRise);
        }
    
};

#warning "Use of mbed with CODAL is not recommended! These classes will not always behave as expected and are provided to attempt to support existing extensions. Please write your extension using CODAL."

#endif
