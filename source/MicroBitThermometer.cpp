/*
The MIT License (MIT)
Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.
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

#include "MicroBitThermometer.h"
#include "codal-core/inc/driver-models/Timer.h"
#include "nrf52.h"

#if CONFIG_ENABLED(MICROBIT_BLE_ENABLED)
#include "nrf_sdm.h"
#endif

using namespace codal;

/*
 * The underlying Nordic libraries that support BLE do not compile cleanly with the stringent GCC settings we employ
 * If we're compiling under GCC, then we suppress any warnings generated from this code (but not the rest of the DAL)
 * The ARM cc compiler is more tolerant. We don't test __GNUC__ here to detect GCC as ARMCC also typically sets this
 * as a compatability option, but does not support the options used...
 */

/**
  * Constructor.
  * Create new MicroBitThermometer that gives an indication of the current temperature.
  *
  * @param id the unique EventModel id of this component. Defaults to DEVICE_ID_THERMOMETER.
  *
  * @code
  * MicroBitThermometer thermometer;
  * @endcode
  */
MicroBitThermometer::MicroBitThermometer(uint16_t id)
{
    this->id = id;
    this->samplePeriod = MICROBIT_THERMOMETER_PERIOD;
    this->sampleTime = 0;
    this->offset = 0;
}

/**
  * Gets the current temperature of the microbit.
  *
  * @return the current temperature, in degrees celsius.
  *
  * @code
  * thermometer.getTemperature();
  * @endcode
  */
int MicroBitThermometer::getTemperature()
{
    updateSample();
    return temperature - offset;
}


/**
  * Updates the temperature sample of this instance of MicroBitThermometer
  * only if isSampleNeeded() indicates that an update is required.
  *
  * This call also will add the thermometer to fiber components to receive
  * periodic callbacks.
  *
  * @return DEVICE_OK on success.
  */
int MicroBitThermometer::updateSample()
{
    // Ensure we're registered for a background processing callbacks.
    status |= DEVICE_COMPONENT_STATUS_IDLE_TICK;

    // check if we need to update our sample...
    if(isSampleNeeded())
    {
        int32_t processorTemperature;
        uint8_t sd_enabled = 0;

        // For now, we just rely on the nrf senesor to be the most accurate.
        // The compass module also has a temperature sensor, and has the lowest power consumption, so will run the cooler...
        // ...however it isn't trimmed for accuracy during manufacture, so requires calibration.

#if CONFIG_ENABLED(MICROBIT_BLE_ENABLED)
        sd_softdevice_is_enabled(&sd_enabled);
#endif
        if (sd_enabled)
        {
            // If Bluetooth is enabled, we need to go through the Nordic software to safely do this
#if CONFIG_ENABLED(MICROBIT_BLE_ENABLED)
            sd_temp_get(&processorTemperature);
#endif
        }
        else
        {
            // Othwerwise, we access the information directly...
            NRF_TEMP->TASKS_START = 1;
            while (NRF_TEMP->EVENTS_DATARDY == 0);
            NRF_TEMP->EVENTS_DATARDY = 0;

            processorTemperature = NRF_TEMP->TEMP;

            NRF_TEMP->TASKS_STOP = 1;
        }


        // Record our reading...
        temperature = processorTemperature / 4;

        // Schedule our next sample.
        sampleTime = system_timer_current_time() + samplePeriod;

        // Send an event to indicate that we'e updated our temperature.
        Event e(id, MICROBIT_THERMOMETER_EVT_UPDATE);
    }

    return DEVICE_OK;
};

/**
  * Periodic callback from MicroBit idle thread.
  */
void MicroBitThermometer::idleCallback()
{
    updateSample();
}

/**
  * Determines if we're due to take another temperature reading
  *
  * @return 1 if we're due to take a temperature reading, 0 otherwise.
  */
int MicroBitThermometer::isSampleNeeded()
{
    return  system_timer_current_time() >= sampleTime;
}

/**
  * Set the sample rate at which the temperatureis read (in ms).
  *
  * The default sample period is 1 second.
  *
  * @param period the requested time between samples, in milliseconds.
  *
  * @note the temperature is always read in the background, and is only updated
  * when the processor is idle, or when the temperature is explicitly read.
  */
void MicroBitThermometer::setPeriod(int period)
{
    updateSample();
    samplePeriod = period;
}

/**
  * Reads the currently configured sample rate of the thermometer.
  *
  * @return The time between samples, in milliseconds.
  */
int MicroBitThermometer::getPeriod()
{
    return samplePeriod;
}

/**
  * Set the value that is used to offset the raw silicon temperature.
  *
  * @param offset the offset for the silicon temperature
  *
  * @return DEVICE_OK on success
  */
int MicroBitThermometer::setCalibration(int offset)
{
    this->offset = offset;
    return DEVICE_OK;
}

/**
  * Retreive the value that is used to offset the raw silicon temperature.
  *
  * @return the current offset.
  */
int MicroBitThermometer::getCalibration()
{
    return offset;
}
