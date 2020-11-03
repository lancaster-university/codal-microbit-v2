#ifndef DigitalOut_h
#define DigitalOut_h

#include "Pin.h"

class DigitalOut: public codal::NRF52Pin {
    public:
        DigitalOut(PinName pin) : codal::NRF52Pin(pin, pin, PIN_CAPABILITY_DIGITAL) 
        {
        }

        void mode(PullMode pull) {
            this->setPull(pull);
        }

        void mode(int pull) {
            switch(pull) {
                case PullNone:
                    this->setPull(PullMode::None);
                    break;
                case PullDown:
                    this->setPull(PullMode::Down);
                    break;
                case PullUp:
                    this->setPull(PullMode::Up);
                    break;
                default:
                    this->setPull(PullMode::Up);
                    break;
            }
        }

        int read() {
            return this->getDigitalValue();
        }
        
        void write(int value) {
            this->setDigitalValue(value);
            return;
        }
        
        DigitalOut &operator= (int value) {
            this->write(value);
            return *this;
        }

        DigitalOut &operator= (DigitalOut &rhs);

        operator int() {
            return this->getDigitalValue();
        }
};
#endif
