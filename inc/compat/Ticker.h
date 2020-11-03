#ifndef Ticker_h
#define Ticker_h

#include "MemberFunctionCallback.h"
#include "Timer.h"
#include "MicroBitEvent.h"

#define DEVICE_ID_IO_MBED_TICKER 0xE3

class Ticker {
    private:
        uint32_t interval;
        MemberFunctionCallback *func;
        MessageBus bus;

    public:

        Ticker() {
        }

        void onTick(MicroBitEvent e) {
            func->fire(e);
        }

        template<typename T>
        void attach(T* tptr, void (T::*mptr)(void), float s) {
            this->func = func;
            this->interval = (s * 1000000.0f);
bus.listen(DEVICE_ID_IO_MBED_TICKER, 0x0, this, &Ticker::onTick);
            system_timer_event_every_us(interval, DEVICE_ID_IO_MBED_TICKER, 0x0);
        }
        
        template<typename T>
        void attach_us(T* tptr, void (T::*mptr)(void), int us) {
            this->func = func;
            this->interval = (us * 1000);

            bus.listen(DEVICE_ID_IO_MBED_TICKER, 0x0, this, &Ticker::onTick);
            system_timer_event_every_us(interval, DEVICE_ID_IO_MBED_TICKER, 0x0);
        }

        void detach() {
            system_timer_cancel_event(DEVICE_ID_IO_MBED_TICKER, 0x0);
        }
};

#endif
