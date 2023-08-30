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

#ifndef MICROBIT_COMPASS_H
#define MICROBIT_COMPASS_H

#include "MicroBitConfig.h"
#include "MicroBitComponent.h"
#include "CoordinateSystem.h"
#include "MicroBitAccelerometer.h"

namespace codal
{
    /**
     * Class definition for a general e-compass.
     */
    class MicroBitCompass : public Compass
    {
      public:
        static Compass*             driver;           // The instance of a Compass driver.
        MicroBitAccelerometer*      accelerometer;    // The accelerometer to use for tilt compensation.

        /**
         * Constructor.
         * Create a software abstraction of an e-compass.
         *
         * @param coordinateSpace the orientation of the sensor.
         * @param id the unique EventModel id of this component. Defaults to: MICROBIT_ID_COMPASS
         *
         */
        MicroBitCompass(MicroBitI2C &i2c, CoordinateSpace &coordinateSpace, uint16_t id = MICROBIT_ID_COMPASS);
        MicroBitCompass(CoordinateSpace &coordinateSpace, uint16_t id = MICROBIT_ID_COMPASS);

        /**
         * Device autodetection. Scans the given I2C bus for supported compass devices.
         * if found, constructs an appropriate driver and returns it.
         *
         * @param i2c the bus to scan.
         *
         */
        static Compass& autoDetect(MicroBitI2C &i2c);

        /**
         * Configures the device for the sample rate defined
         * in this object. The nearest values are chosen to those defined
         * that are supported by the hardware. The instance variables are then
         * updated to reflect reality.
         *
         * @return MICROBIT_OK on success, MICROBIT_I2C_ERROR if the compass could not be configured.
         */
        virtual int configure() override;

        /**
         * Poll to see if new data is available from the hardware. If so, update it.
         * n.b. it is not necessary to explicitly call this function to update data
         * (it normally happens in the background when the scheduler is idle), but a check is performed
         * if the user explicitly requests up to date data.
         *
         * @return MICROBIT_OK on success, MICROBIT_I2C_ERROR if the update fails.
         */
        virtual int requestUpdate() override;

        /**
         * Destructor.
         */
        ~MicroBitCompass();
    };

    //
    // Backward Compatibility
    //
    typedef Sample3D CompassSample;

}

#endif
