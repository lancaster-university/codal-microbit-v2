#ifndef DigitalIn_h
#define DigitalIn_h
  
#include "Pin.h"

class DigitalIn: public codal::NRF52Pin {
    public:
        DigitalIn(PinName pin) : codal::NRF52Pin(pin, pin, PIN_CAPABILITY_DIGITAL) 
        {
        }

        void mode(PullMode pull) {
            this->setPull(pull);
        }

        int read() {
            return this->getDigitalValue();
        }

        operator int() {
            return this->getDigitalValue();
        }
};

#endif
