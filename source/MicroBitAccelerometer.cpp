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
#include "ErrorNo.h"
#include "MicroBitEvent.h"
#include "MicroBitCompat.h"
#include "MicroBitFiber.h"
#include "MicroBitDevice.h"
#include "MicroBitI2C.h"
#include "MicroBitCompass.h"
#include "FXOS8700.h"
#include "FXOS8700Accelerometer.h"
#include "FXOS8700Magnetometer.h"
#include "LSM303Accelerometer.h"
#include "LSM303Magnetometer.h"

Accelerometer* MicroBitAccelerometer::detectedAccelerometer = NULL;
NRF52Pin MicroBitAccelerometer::irq1(ID_PIN_P25, P0_25, PIN_CAPABILITY_AD);
CoordinateSpace MicroBitAccelerometer::coordinateSpace(SIMPLE_CARTESIAN, true, COORDINATE_SPACE_ROTATED_0);

MicroBitAccelerometer::MicroBitAccelerometer(MicroBitI2C &i2c, uint16_t id) : Accelerometer(coordinateSpace)
{
    autoDetect(i2c);
}
/**
 * Device autodetection. Scans the given I2C bus for supported accelerometer devices.
 * if found, constructs an appropriate driver and returns it.
 *
 * @param i2c the bus to scan. 
 * @param id the unique EventModel id of this component. Defaults to: MICROBIT_ID_ACCELEROMETER
 *
 */
Accelerometer& MicroBitAccelerometer::autoDetect(MicroBitI2C &i2c)
{
    // Add pullup resisitor to IRQ line (it's floating ACTIVE LO)
    irq1.getDigitalValue();
    irq1.setPull(PullUp);
    irq1.setActiveLo();
    
    if (MicroBitAccelerometer::detectedAccelerometer == NULL)
    {
        // All known accelerometer/magnetometer peripherals have the same alignment
        if (FXOS8700::isDetected(i2c, 0x3E))
        {
            FXOS8700 *fxos = new FXOS8700(i2c, irq1, 0x3E);
            MicroBitAccelerometer::detectedAccelerometer = new FXOS8700Accelerometer(*fxos, coordinateSpace);
            MicroBitCompass::detectedCompass = new FXOS8700Magnetometer(*fxos, coordinateSpace);
        }

        // Now, probe for connected peripherals, if none have already been found.
        if (LSM303Accelerometer::isDetected(i2c, 0x32))
        {
            MicroBitAccelerometer::detectedAccelerometer = new LSM303Accelerometer(i2c, irq1, coordinateSpace, 0x32);
            MicroBitCompass::detectedCompass = new LSM303Magnetometer(i2c, irq1, coordinateSpace, 0x3C);
        }
    
        if (MicroBitAccelerometer::detectedAccelerometer == NULL)
            MicroBitAccelerometer::detectedAccelerometer = new Accelerometer(coordinateSpace);

        if (MicroBitCompass::detectedCompass)
            MicroBitCompass::detectedCompass->setAccelerometer(*MicroBitAccelerometer::detectedAccelerometer);
    }
    
    return *detectedAccelerometer;
}

/**
  * Attempts to set the sample rate of the accelerometer to the specified value (in ms).
  *
  * @param period the requested time between samples, in milliseconds.
  *
  * @return MICROBIT_OK on success, MICROBIT_I2C_ERROR is the request fails.
  *
  * @code
  * // sample rate is now 20 ms.
  * accelerometer.setPeriod(20);
  * @endcode
  *
  * @note The requested rate may not be possible on the hardware. In this case, the
  * nearest lower rate is chosen.
  */
int MicroBitAccelerometer::setPeriod(int period)
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->setPeriod(period) : Accelerometer::setPeriod(period);
}

/**
  * Reads the currently configured sample rate of the accelerometer.
  *
  * @return The time between samples, in milliseconds.
  */
int MicroBitAccelerometer::getPeriod()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getPeriod() : Accelerometer::getPeriod();
}

/**
  * Attempts to set the sample range of the accelerometer to the specified value (in g).
  *
  * @param range The requested sample range of samples, in g.
  *
  * @return MICROBIT_OK on success, MICROBIT_I2C_ERROR is the request fails.
  *
  * @code
  * // the sample range of the accelerometer is now 8G.
  * accelerometer.setRange(8);
  * @endcode
  *
  * @note The requested range may not be possible on the hardware. In this case, the
  * nearest lower range is chosen.
  */
int MicroBitAccelerometer::setRange(int range)
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->setRange(range) : Accelerometer::setRange(range);
}

/**
  * Reads the currently configured sample range of the accelerometer.
  *
  * @return The sample range, in g.
  */
int MicroBitAccelerometer::getRange()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getRange() : Accelerometer::getRange();
}

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
int MicroBitAccelerometer::configure()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->configure() : Accelerometer::configure();
}

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
int MicroBitAccelerometer::requestUpdate()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->requestUpdate() : Accelerometer::requestUpdate();
}

/**
 * Reads the last accelerometer value stored, and provides it in the coordinate system requested.
 *
 * @param coordinateSpace The coordinate system to use.
 * @return The force measured in each axis, in milli-g.
 */
Sample3D MicroBitAccelerometer::getSample(CoordinateSystem coordinateSystem)
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getSample(coordinateSystem) : Accelerometer::getSample(coordinateSystem);
}

/**
 * Reads the last accelerometer value stored, and in the coordinate system defined in the constructor.
 * @return The force measured in each axis, in milli-g.
 */
Sample3D MicroBitAccelerometer::getSample()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getSample() : Accelerometer::getSample();
}

/**
 * reads the value of the x axis from the latest update retrieved from the accelerometer,
 * usingthe default coordinate system as specified in the constructor.
 *
 * @return the force measured in the x axis, in milli-g.
 */
int MicroBitAccelerometer::getX()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getX() : Accelerometer::getX();
}

/**
 * reads the value of the y axis from the latest update retrieved from the accelerometer,
 * usingthe default coordinate system as specified in the constructor.
 *
 * @return the force measured in the y axis, in milli-g.
 */
int MicroBitAccelerometer::getY()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getY() : Accelerometer::getY();
}

/**
 * reads the value of the z axis from the latest update retrieved from the accelerometer,
 * usingthe default coordinate system as specified in the constructor.
 *
 * @return the force measured in the z axis, in milli-g.
 */
int MicroBitAccelerometer::getZ()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getZ() : Accelerometer::getZ();
}

/**
  * Provides a rotation compensated pitch of the device, based on the latest update retrieved from the accelerometer.
  *
  * @return The pitch of the device, in degrees.
  *
  * @code
  * accelerometer.getPitch();
  * @endcode
  */
int MicroBitAccelerometer::getPitch()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getPitch() : Accelerometer::getPitch();
}

/**
  * Provides a rotation compensated pitch of the device, based on the latest update retrieved from the accelerometer.
  *
  * @return The pitch of the device, in radians.
  *
  * @code
  * accelerometer.getPitchRadians();
  * @endcode
  */
float MicroBitAccelerometer::getPitchRadians()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getPitchRadians() : Accelerometer::getPitchRadians();
}

/**
  * Provides a rotation compensated roll of the device, based on the latest update retrieved from the accelerometer.
  *
  * @return The roll of the device, in degrees.
  *
  * @code
  * accelerometer.getRoll();
  * @endcode
  */
int MicroBitAccelerometer::getRoll()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getRoll() : Accelerometer::getRoll();
}

/**
  * Provides a rotation compensated roll of the device, based on the latest update retrieved from the accelerometer.
  *
  * @return The roll of the device, in radians.
  *
  * @code
  * accelerometer.getRollRadians();
  * @endcode
  */
float MicroBitAccelerometer::getRollRadians()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getRollRadians() : Accelerometer::getRollRadians();
}

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
uint16_t MicroBitAccelerometer::getGesture()
{
    return MicroBitAccelerometer::detectedAccelerometer ? detectedAccelerometer->getGesture() : Accelerometer::getGesture();
}

/**
  * Destructor for FXS8700, where we deregister from the array of fiber components.
  */
MicroBitAccelerometer::~MicroBitAccelerometer()
{
}
