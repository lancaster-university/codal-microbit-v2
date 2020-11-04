#ifndef Timeout_h
#define Timeout_h

#include "MbedMemberFunctionCallback.h"
#include "Timer.h"
#include "MicroBitEvent.h"
#include "MessageBus.h"

#define DEVICE_ID_MBED_TIMEOUT 0xE2

class Timeout {

    private:
        uint32_t interval;
        MbedMemberFunctionCallback *func;
        MessageBus bus;
            
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

            bus.listen(DEVICE_ID_MBED_TIMEOUT, 0x0, this, &Timeout::onTimeout);
            system_timer_event_after_us(interval, DEVICE_ID_MBED_TIMEOUT, 0x0);
        }
        
        template<typename T>
        void attach_us(T* tptr, void (T::*mptr)(void), int us) {
            this->func = new MbedMemberFunctionCallback(tptr, mptr);
            this->interval = (us * 1000);

            bus.listen(DEVICE_ID_MBED_TIMEOUT, 0x0, this, &Timeout::onTimeout);
            system_timer_event_after_us(interval, DEVICE_ID_MBED_TIMEOUT, 0x0);
        }

        void detach() {
            system_timer_cancel_event(DEVICE_ID_MBED_TIMEOUT, 0x0);
        }

};

#endif
