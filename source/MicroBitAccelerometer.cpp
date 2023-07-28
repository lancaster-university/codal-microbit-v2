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

#include "MicroBitAccelerometer.h"
#include "MicroBitIO.h"
#include "MicroBitEvent.h"
#include "MicroBitCompat.h"
#include "MicroBitDevice.h"
#include "MicroBitI2C.h"
#include "MicroBitCompass.h"
#include "MicroBitError.h"
#include "LSM303Accelerometer.h"
#include "LSM303Magnetometer.h"


using namespace codal;

Accelerometer* MicroBitAccelerometer::driver = NULL;


MicroBitAccelerometer::MicroBitAccelerometer(MicroBitI2C &i2c, CoordinateSpace &coordinateSpace, uint16_t id) : Accelerometer(coordinateSpace, id)
{
    autoDetect(i2c);
}

MicroBitAccelerometer::MicroBitAccelerometer(CoordinateSpace &coordinateSpace, uint16_t id) : Accelerometer(coordinateSpace, id)
{
}

Accelerometer& MicroBitAccelerometer::autoDetect(MicroBitI2C &i2c)
{
    static bool autoDetectCompleted = false;
    static CoordinateSpace coordinateSpace(SIMPLE_CARTESIAN, true, COORDINATE_SPACE_ROTATED_0);
    static NRF52Pin irq1(ID_PIN_IRQ1, P0_25, PIN_CAPABILITY_AD);

    if (!autoDetectCompleted)
    {
        /*
        * In essence, the LSM needs at least 6.4ms from power-up before we can use it.
        * https://github.com/microbit-foundation/codal-microbit/issues/33
        */
        target_wait(7);

        // Add pullup resistor to IRQ line (it's floating ACTIVE LO)
        irq1.getDigitalValue();
        irq1.setPull(PullMode::Up);
        irq1.setActiveLo();

        // Now, probe for the LSM303, and if it doesn't reply, panic
        if ( LSM303Accelerometer::isDetected(i2c, LSM303_A_DEFAULT_ADDR) )
        {
            MicroBitAccelerometer::driver = new LSM303Accelerometer( i2c, irq1, coordinateSpace, LSM303_A_DEFAULT_ADDR );
            MicroBitCompass::driver = new LSM303Magnetometer( i2c, irq1, coordinateSpace, LSM303_M_DEFAULT_ADDR );
        }
        else
        {
            MicroBitAccelerometer::driver = new MicroBitAccelerometer( coordinateSpace );
            MicroBitCompass::driver = new MicroBitCompass( coordinateSpace );
        }
        MicroBitCompass::driver->setAccelerometer( *MicroBitAccelerometer::driver );

        autoDetectCompleted = true;
    }

    return *driver;
}

int MicroBitAccelerometer::configure()
{
    if ( MicroBitAccelerometer::driver == this )
    {
        microbit_panic( MicroBitPanic::ACCELEROMETER_ERROR );
        return DEVICE_NOT_SUPPORTED;
    }
    else
    {
        // Using an else helps the compiler to compile out this bit if possible
        return driver->configure();
    }
}

int MicroBitAccelerometer::requestUpdate()
{
    if ( MicroBitAccelerometer::driver == this )
    {
        microbit_panic( MicroBitPanic::ACCELEROMETER_ERROR );
        return DEVICE_NOT_SUPPORTED;
    }
    else
    {
        // Using an else helps the compiler to compile out this bit if possible
        return driver->requestUpdate();
    }
}

MicroBitAccelerometer::~MicroBitAccelerometer()
{
}
