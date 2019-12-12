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
#include "codal-core/inc/core/CodalComponent.h"
#include "codal-core/inc/driver-models/Accelerometer.h"
#include "codal-core/inc/driver-models/Compass.h"
#include "codal-core/inc/driver-models/I2C.h"
#include "codal-core/inc/driver-models/Pin.h"
#include "codal-core/inc/types/CoordinateSystem.h"


/**
 * Class definition for MicroBitAccelerometer.
 */
class MicroBitAccelerometer : public Accelerometer
{
    public:

        static Accelerometer *detectedAccelerometer;      // The autodetected instance of an Accelerometer driver.
        static NRF52Pin irq1;                             // IRQ pin for detected acceleromters to use
        static CoordinateSpace coordinateSpace;           // Default coordinate space
    
        /**
         * Constructor.
         * Create a software abstraction of an accelerometer.
         *
         * @param coordinateSpace the orientation of the sensor. Defaults to: SIMPLE_CARTESIAN
         * @param id the unique EventModel id of this component. Defaults to: MICROBIT_ID_ACCELEROMETER
         *
         */
        MicroBitAccelerometer(MicroBitI2C &i2c, uint16_t id = MICROBIT_ID_ACCELEROMETER);

        /**
         * Device autodetection. Scans the given I2C bus for supported accelerometer devices.
         * if found, constructs an appropriate driver and returns it.
         *
         * @param i2c the bus to scan. 
         *
         */
        static Accelerometer& autoDetect(MicroBitI2C &i2c); 

        /**
         * Attempts to set the sample rate of the accelerometer to the specified value (in ms).
         *
         * @param period the requested time between samples, in milliseconds.
         * @return MICROBIT_OK on success, MICROBIT_I2C_ERROR is the request fails.
         *
         * @note The requested rate may not be possible on the hardware. In this case, the
         * nearest lower rate is chosen.
         *
         * @note This method should be overriden (if supported) by specific accelerometer device drivers.
         */
        int setPeriod(int period);

        /**
         * Reads the currently configured sample rate of the accelerometer.
         *
         * @return The time between samples, in milliseconds.
         */
        int getPeriod();

        /**
         * Attempts to set the sample range of the accelerometer to the specified value (in g).
         *
         * @param range The requested sample range of samples, in g.
         *
         * @return MICROBIT_OK on success, MICROBIT_I2C_ERROR is the request fails.
         *
         * @note The requested range may not be possible on the hardware. In this case, the
         * nearest lower range is chosen.
         *
         * @note This method should be overriden (if supported) by specific accelerometer device drivers.
         */
        int setRange(int range);

        /**
         * Reads the currently configured sample range of the accelerometer.
         *
         * @return The sample range, in g.
         */
        int getRange();

        /**
         * Configures the accelerometer for G range and sample rate defined
         * in this object. The nearest values are chosen to those defined
         * that are supported by the hardware. The instance variables are then
         * updated to reflect reality.
         *
         * @return MICROBIT_OK on success, MICROBIT_I2C_ERROR if the accelerometer could not be configured.
         *
         * @note This method should be overidden by the hardware driver to implement the requested
         * changes in hardware.
         */
        int configure();

        /**
         * Poll to see if new data is available from the hardware. If so, update it.
         * n.b. it is not necessary to explicitly call this funciton to update data
         * (it normally happens in the background when the scheduler is idle), but a check is performed
         * if the user explicitly requests up to date data.
         *
         * @return MICROBIT_OK on success, MICROBIT_I2C_ERROR if the update fails.
         *
         * @note This method should be overidden by the hardware driver to implement the requested
         * changes in hardware.
         */
        int requestUpdate();

        /**
         * Reads the last accelerometer value stored, and provides it in the coordinate system requested.
         *
         * @param coordinateSpace The coordinate system to use.
         * @return The force measured in each axis, in milli-g.
         */
        Sample3D getSample(CoordinateSystem coordinateSystem);

        /**
         * Reads the last accelerometer value stored, and in the coordinate system defined in the constructor.
         * @return The force measured in each axis, in milli-g.
         */
        Sample3D getSample();

        /**
         * reads the value of the x axis from the latest update retrieved from the accelerometer,
         * using the default coordinate system as specified in the constructor.
         *
         * @return the force measured in the x axis, in milli-g.
         */
        int getX();

        /**
         * reads the value of the y axis from the latest update retrieved from the accelerometer,
         * using the default coordinate system as specified in the constructor.
         *
         * @return the force measured in the y axis, in milli-g.
         */
        int getY();

        /**
         * reads the value of the z axis from the latest update retrieved from the accelerometer,
         * using the default coordinate system as specified in the constructor.
         *
         * @return the force measured in the z axis, in milli-g.
         */
        int getZ();

        /**
         * Provides a rotation compensated pitch of the device, based on the latest update retrieved from the accelerometer.
         *
         * @return The pitch of the device, in degrees.
         *
         * @code
         * accelerometer.getPitch();
         * @endcode
         */
        int getPitch();

        /**
         * Provides a rotation compensated pitch of the device, based on the latest update retrieved from the accelerometer.
         *
         * @return The pitch of the device, in radians.
         *
         * @code
         * accelerometer.getPitchRadians();
         * @endcode
         */
        float getPitchRadians();

        /**
         * Provides a rotation compensated roll of the device, based on the latest update retrieved from the accelerometer.
         *
         * @return The roll of the device, in degrees.
         *
         * @code
         * accelerometer.getRoll();
         * @endcode
         */
        int getRoll();

        /**
         * Provides a rotation compensated roll of the device, based on the latest update retrieved from the accelerometer.
         *
         * @return The roll of the device, in radians.
         *
         * @code
         * accelerometer.getRollRadians();
         * @endcode
         */
        float getRollRadians();

        /**
         * Retrieves the last recorded gesture.
         *
         * @return The last gesture that was detected.
         *
         * Example:
         * @code
         *
         * if (accelerometer.getGesture() == SHAKE)
         *     display.scroll("SHAKE!");
         * @endcode
         */
        uint16_t getGesture();

        /**
         * Destructor.
         */
        ~MicroBitAccelerometer();
};

#endif
