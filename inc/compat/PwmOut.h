#ifndef PwmOut_h
#define PwmOut_h

#include "MicroBitIO.h"
#include "NRF52Pin.h"

class PwmOut {
    public:
        PwmOut(PinName pin) {
            p(DEVICE_ID_IO_P0, (int)pin->name, PIN_CAPABILITY_BOTH);
            return;
        }

        void write(float width) {
            // _p.setAnalogValue(width * 1024.0);
        }
};

#endif
