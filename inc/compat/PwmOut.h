#ifndef PwmOut_h
#define PwmOut_h

#include "MicroBitCompat.h"
#include "MicroBitIO.h"
#include "NRF52Pin.h"

class PwmOut {

    private: 
        NRF52Pin p;

    public:
        PwmOut(PinName pin) : p(MICROBIT_ID_MBED_PWM, pin, PIN_CAPABILITY_ANALOG) {
            return;
        }

        void write(float width) {
            p.setAnalogValue(width * 1024.0);
        }

        void period_us(float period) {
            p.setAnalogPeriod(period);
        }
};

#warning "Use of mbed with CODAL is not recommended! These classes will not always behave as expected and are provided to attempt to support existing extensions. Please write your extension using CODAL."

#endif
