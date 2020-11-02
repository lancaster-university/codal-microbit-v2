#ifndef Timeout_h
#define Timeout_h

#include "MemberFunctionCallback.h"

class Timeout {
    private:
        void *func;
        int interval;

    public:
        template<typename T>
        void attach(T* tptr, void (T::*mptr)(void), float s) {
            this->func = func;
            this->interval = (s * 1000000.0f);
        }
        
        template<typename T>
        void attach_us(T* tptr, void (T::*mptr)(void), int us) {
            this->func = func;
            this->interval = (us * 1000);
            // create_fiber(ticker_thread);
        }

        void detach() {
            // release_fiber(ticker_thread);
        }
};

#endif
