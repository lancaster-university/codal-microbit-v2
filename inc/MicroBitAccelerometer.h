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

#ifndef MICROBIT_ACCELEROMTER_H
#define MICROBIT_ACCELEROMTER_H

#include "CodalConfig.h"
#include "MicroBitCompat.h"
#include "codal-core/inc/driver-models/Accelerometer.h"
#include "codal-core/inc/types/CoordinateSystem.h"
#include "MicroBitI2C.h"


namespace codal
{
    /**
     * Class definition for MicroBitAccelerometer.
     */
    class MicroBitAccelerometer : public Accelerometer
    {
      public:

        static Accelerometer* driver;                     // The instance of an Accelerometer driver.

        /**
         * Constructor.
         * Create an instance of an accelerometer that detects which on-board
         * sensor is present and initialises the respective driver.
         *
         * @param coordinateSpace the orientation of the sensor.
         * @param id the unique EventModel id of this component. Defaults to: MICROBIT_ID_ACCELEROMETER
         *
         */
        MicroBitAccelerometer(MicroBitI2C &i2c, CoordinateSpace &coordinateSpace, uint16_t id = MICROBIT_ID_ACCELEROMETER);

        /**
         * Constructor for an empty driver.
         *
         * This constructor should only be called when the on-board sensor is not
         * detected, and we need an instance that's able to panic on first usage.
         *
         * @param coordinateSpace the orientation of the sensor.
         * @param id the unique EventModel id of this component. Defaults to: MICROBIT_ID_ACCELEROMETER
         *
         */
        MicroBitAccelerometer(CoordinateSpace &coordinateSpace, uint16_t id = MICROBIT_ID_ACCELEROMETER);

        /**
         * Device autodetection. Scans the given I2C bus for supported accelerometer devices.
         * if found, constructs an appropriate driver and returns it.
         *
         * @param i2c the bus to scan.
         *
         */
        static Accelerometer& autoDetect(MicroBitI2C &i2c); 

        /**
         * Configures the accelerometer for G range and sample rate defined
         * in this object. The nearest values are chosen to those defined
         * that are supported by the hardware. The instance variables are then
         * updated to reflect reality.
         *
         * @return MICROBIT_OK on success, MICROBIT_I2C_ERROR if the accelerometer could not be configured.
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
        ~MicroBitAccelerometer();
    };
}

#endif
