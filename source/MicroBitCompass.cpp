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
#include "LSM303Magnetometer.h"

Compass* MicroBitCompass::detectedCompass= NULL;

/**
 * Constructor.
 * Create a software abstraction of an e-compass.
 *
 * @param id the unique EventModel id of this component. Defaults to: MICROBIT_ID_COMPASS
 * @param coordinateSpace the orientation of the sensor. Defaults to: SIMPLE_CARTESIAN
 *
 */
MicroBitCompass::MicroBitCompass(MicroBitI2C &i2c, uint16_t id) : Compass(MicroBitAccelerometer::coordinateSpace)
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
Compass& MicroBitCompass::autoDetect(MicroBitI2C &i2c)
{
    /*
     * In essence, the LSM needs at least 6.4ms from power-up before we can use it.
     * https://github.com/microbit-foundation/codal-microbit/issues/33
     */
    target_wait(10);
    // We only have combined sensors, so rely on the acceleromter detection code to also detect the correct magnetomter.
    if (MicroBitCompass::detectedCompass == NULL)
        MicroBitAccelerometer::autoDetect(i2c);

    if (MicroBitCompass::detectedCompass == NULL)
    {
        CoordinateSpace c(CoordinateSystem::RAW);
        MicroBitCompass::detectedCompass = new Compass(c);
    }

    return *MicroBitCompass::detectedCompass;

}

/**
 * Gets the current heading of the device, relative to magnetic north.
 *
 * If the compass is not calibrated, it will raise the MICROBIT_COMPASS_EVT_CALIBRATE event.
 *
 * Users wishing to implement their own calibration algorithms should listen for this event,
 * using MESSAGE_BUS_LISTENER_IMMEDIATE model. This ensures that calibration is complete before
 * the user program continues.
 *
 * @return the current heading, in degrees. Or CALIBRATION_IN_PROGRESS if the compass is calibrating.
 *
 * @code
 * compass.heading();
 * @endcode
 */
int MicroBitCompass::heading()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->heading() : Compass::heading();
}

/**
 * Determines the overall magnetic field strength based on the latest update from the magnetometer.
 *
 * @return The magnetic force measured across all axis, in nano teslas.
 *
 * @code
 * compass.getFieldStrength();
 * @endcode
 */
int MicroBitCompass::getFieldStrength()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->getFieldStrength() : Compass::getFieldStrength();
}

/**
 * Perform a calibration of the compass.
 *
 * This method will be called automatically if a user attempts to read a compass value when
 * the compass is uncalibrated. It can also be called at any time by the user.
 *
 * The method will only return once the compass has been calibrated.
 *
 * @return MICROBIT_OK, MICROBIT_I2C_ERROR if the magnetometer could not be accessed,
 * or MICROBIT_CALIBRATION_REQUIRED if the calibration algorithm failed to complete successfully.
 *
 * @note THIS MUST BE CALLED TO GAIN RELIABLE VALUES FROM THE COMPASS
 */
int MicroBitCompass::calibrate()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->calibrate() : Compass::calibrate();
}

/**
 * Configure the compass to use the calibration data that is supplied to this call.
 *
 * Calibration data is comprised of the perceived zero offset of each axis of the compass.
 *
 * After calibration this should now take into account trimming errors in the magnetometer,
 * and any "hard iron" offsets on the device.
 *
 * @param calibration A Sample3D containing the offsets for the x, y and z axis.
 */
void MicroBitCompass::setCalibration(CompassCalibration calibration)
{
    return MicroBitCompass::detectedCompass ? detectedCompass->setCalibration(calibration) : Compass::setCalibration(calibration);
}

/**
 * Provides the calibration data currently in use by the compass.
 *
 * More specifically, the x, y and z zero offsets of the compass.
 *
 * @return A Sample3D containing the offsets for the x, y and z axis.
 */
CompassCalibration MicroBitCompass::getCalibration()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->getCalibration() : Compass::getCalibration();
}

/**
 * Returns 0 or 1. 1 indicates that the compass is calibrated, zero means the compass requires calibration.
 */
int MicroBitCompass::isCalibrated()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->isCalibrated() : Compass::isCalibrated();
}

/**
 * Returns 0 or 1. 1 indicates that the compass is calibrating, zero means the compass is not currently calibrating.
 */
int MicroBitCompass::isCalibrating()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->isCalibrating() : Compass::isCalibrating();
}

/**
 * Clears the calibration held in memory storage, and sets the calibrated flag to zero.
 */
void MicroBitCompass::clearCalibration()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->clearCalibration() : Compass::clearCalibration();
}

/**
 * Configures the device for the sample rate defined
 * in this object. The nearest values are chosen to those defined
 * that are supported by the hardware. The instance variables are then
 * updated to reflect reality.
 *
 * @return MICROBIT_OK on success, MICROBIT_I2C_ERROR if the compass could not be configured.
 */
int MicroBitCompass::configure()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->configure() : Compass::configure();
}

/**
 *
 * Defines the accelerometer to be used for tilt compensation.
 *
 * @param acceleromter Reference to the accelerometer to use.
 */
void MicroBitCompass::setAccelerometer(MicroBitAccelerometer &accelerometer)
{
    return MicroBitCompass::detectedCompass ? detectedCompass->setAccelerometer(accelerometer) : Compass::setAccelerometer(accelerometer);
}

/**
 * Attempts to set the sample rate of the compass to the specified period value (in ms).
 *
 * @param period the requested time between samples, in milliseconds.
 * @return MICROBIT_OK on success, MICROBIT_I2C_ERROR is the request fails.
 *
 * @note The requested rate may not be possible on the hardware. In this case, the
 * nearest lower rate is chosen.
 *
 * @note This method should be overriden (if supported) by specific magnetometer device drivers.
 */
int MicroBitCompass::setPeriod(int period)
{
    return MicroBitCompass::detectedCompass ? detectedCompass->setPeriod(period) : Compass::setPeriod(period);
}

/**
 * Reads the currently configured sample rate of the compass.
 *
 * @return The time between samples, in milliseconds.
 */
int MicroBitCompass::getPeriod()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->getPeriod() : Compass::getPeriod();
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
int MicroBitCompass::requestUpdate()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->requestUpdate() : Compass::requestUpdate();
}

/**
 * Reads the last compass value stored, and provides it in the coordinate system requested.
 *
 * @param coordinateSpace The coordinate system to use.
 * @return The force measured in each axis, in milli-g.
 */
Sample3D MicroBitCompass::getSample(CoordinateSystem coordinateSystem)
{
    return MicroBitCompass::detectedCompass ? detectedCompass->getSample(coordinateSystem) : Compass::getSample(coordinateSystem);
}

/**
 * Reads the last compass value stored, and in the coordinate system defined in the constructor.
 * @return The force measured in each axis, in milli-g.
 */
Sample3D MicroBitCompass::getSample()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->getSample() : Compass::getSample();
}

/**
 * reads the value of the x axis from the latest update retrieved from the compass,
 * using the default coordinate system as specified in the constructor.
 *
 * @return the force measured in the x axis, in milli-g.
 */
int MicroBitCompass::getX()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->getX() : Compass::getX();
}

/**
 * reads the value of the y axis from the latest update retrieved from the compass,
 * using the default coordinate system as specified in the constructor.
 *
 * @return the force measured in the y axis, in milli-g.
 */
int MicroBitCompass::getY()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->getY() : Compass::getY();
}

/**
 * reads the value of the z axis from the latest update retrieved from the compass,
 * using the default coordinate system as specified in the constructor.
 *
 * @return the force measured in the z axis, in milli-g.
 */
int MicroBitCompass::getZ()
{
    return MicroBitCompass::detectedCompass ? detectedCompass->getY() : Compass::getZ();
}

/**
  * Destructor.
  */
MicroBitCompass::~MicroBitCompass()
{
}

