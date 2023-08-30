/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "MicroBitCompass.h"
#include "ErrorNo.h"
#include "MicroBitEvent.h"
#include "MicroBitCompat.h"
#include "MicroBitFiber.h"
#include "MicroBitDevice.h"
#include "MicroBitError.h"
#include "LSM303Magnetometer.h"

using namespace codal;

Compass* MicroBitCompass::driver = NULL;

MicroBitCompass::MicroBitCompass(MicroBitI2C &i2c, CoordinateSpace &coordinateSpace, uint16_t id) : Compass(coordinateSpace, id)
{
    autoDetect(i2c);
}

MicroBitCompass::MicroBitCompass(CoordinateSpace &coordinateSpace, uint16_t id) : Compass(coordinateSpace, id)
{
}

Compass& MicroBitCompass::autoDetect(MicroBitI2C &i2c)
{
    // We only have combined sensors, so rely on the accelerometer detection code to also detect the magnetometer.
    MicroBitAccelerometer::autoDetect(i2c);

    return *MicroBitCompass::driver;
}

int MicroBitCompass::configure()
{
    if ( MicroBitCompass::driver == this )
    {
        microbit_panic( MicroBitPanic::COMPASS_ERROR );
        return DEVICE_NOT_SUPPORTED;
    }
    else
    {
        // Using an else helps the compiler to compile out this bit if possible
        return driver->configure();
    }
}

int MicroBitCompass::requestUpdate()
{
    if ( MicroBitCompass::driver == this )
    {
        microbit_panic( MicroBitPanic::COMPASS_ERROR );
        return DEVICE_NOT_SUPPORTED;
    }
    else
    {
        // Using an else helps the compiler to compile out this bit if possible
        return driver->requestUpdate();
    }
}

MicroBitCompass::~MicroBitCompass()
{
}
