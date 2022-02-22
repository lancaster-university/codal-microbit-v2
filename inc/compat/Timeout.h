#ifndef Timeout_h
#define Timeout_h

/* uBit object from PXT or CODAL
 * TODO: MICROBIT_DAL_PXT will be added to PXT to replace this define */
#ifdef MICROBIT_DAL_FIBER_USER_DATA
#include "pxt.h"
#else
extern MicroBit uBit;
#endif

#include "MicroBitCompat.h"
#include "MbedMemberFunctionCallback.h"
#include "Timer.h"
#include "MicroBitEvent.h"

class Timeout {

    private:
        uint32_t interval;
        MbedMemberFunctionCallback *func;
            
    public:

        Timeout() {
        }
        
        void onTimeout(MicroBitEvent e) {
            func->fire();
        }

        template<typename T>
        void attach(T* tptr, void (T::*mptr)(void), float s) {
            this->func = new MbedMemberFunctionCallback(tptr, mptr);
            this->interval = (s * 1000000.0f);

            uBit.messageBus.listen(MICROBIT_ID_MBED_TIMEOUT, 0x0, this, &Timeout::onTimeout);
            system_timer_event_after_us(interval, MICROBIT_ID_MBED_TIMEOUT, 0x0);
        }
        
        template<typename T>
        void attach_us(T* tptr, void (T::*mptr)(void), int us) {
            this->func = new MbedMemberFunctionCallback(tptr, mptr);
            this->interval = (us * 1000);

            uBit.messageBus.listen(MICROBIT_ID_MBED_TIMEOUT, 0x0, this, &Timeout::onTimeout);
            system_timer_event_after_us(interval, MICROBIT_ID_MBED_TIMEOUT, 0x0);
        }

        void detach() {
            system_timer_cancel_event(MICROBIT_ID_MBED_TIMEOUT, 0x0);
        }

};

#warning "Use of mbed with CODAL is not recommended! These classes will not always behave as expected and are provided to attempt to support existing extensions. Please write your extension using CODAL."

#endif
