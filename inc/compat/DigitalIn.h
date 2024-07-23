#ifndef DigitalIn_h
#define DigitalIn_h
  
#include "Pin.h"

class DigitalIn: public codal::NRF52Pin {
    public:
        DigitalIn(PinName pin) : codal::NRF52Pin(pin, pin, codal::PIN_CAPABILITY_DIGITAL) 
        {
        }

        void mode(codal::PullMode pull) {
            this->setPull(pull);
        }

        void mode(int pull) {
            switch(pull) {
                case PullNone:
                    this->setPull(codal::PullMode::None);
                    break;
                case PullDown:
                    this->setPull(codal::PullMode::Down);
                    break;
                case PullUp:
                    this->setPull(codal::PullMode::Up);
                    break;
                default:
                    this->setPull(codal::PullMode::Up);
                    break;
            }
        }

        int read() {
            return this->getDigitalValue();
        }

        operator int() {
            return this->getDigitalValue();
        }
};

#warning "Use of mbed with CODAL is not recommended! These classes will not always behave as expected and are provided to attempt to support existing extensions. Please write your extension using CODAL."

#endif
