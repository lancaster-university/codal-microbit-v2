#ifndef PwmOut_h
#define PwmOut_h

#include "MicroBitIO.h"
#include "NRF52Pin.h"

#define DEVICE_ID_MBED_PWM          0xE1

class PwmOut {

    private: 
        NRF52Pin p;

    public:
        PwmOut(PinName pin) : p(DEVICE_ID_MBED_PWM, pin, PIN_CAPABILITY_ANALOG) {
            return;
        }

        void write(float width) {
            p.setAnalogValue(width * 1024.0);
        }

        void period_us(float period) {
            p.setAnalogPeriod(period);
        }
};

#endif
