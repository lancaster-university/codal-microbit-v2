/*
The MIT License (MIT)

Copyright (c) 2016 Lancaster University, UK.

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

#ifndef MICROBIT_H
#define MICROBIT_H

#include "mbed.h"

#include "MicroBitConfig.h"
#include "CodalHeapAllocator.h"
#include "codal-core/inc/types/Event.h"
#include "CodalDevice.h"
#include "ErrorNo.h"
#include "MbedTimer.h"
#include "Matrix4.h"
#include "CodalCompat.h"
#include "CodalComponent.h"
#include "CodalDmesg.h"
#include "ManagedType.h"
#include "ManagedString.h"
#include "NotifyEvents.h"

#include "Button.h"
#include "MultiButton.h"
#include "NRF52Pin.h"
#include "MbedI2C.h"

#include "MbedSerial.h"
#include "MicroBitIO.h"
#include "MicroBitDisplay.h"
#include "CodalFiber.h"
#include "MessageBus.h"
#include "FXOS8700.h"
#include "FXOS8700Accelerometer.h"
#include "FXOS8700Magnetometer.h"
#include "MicroBitRadio.h"
#include "MicroBitThermometer.h"
#include "MicroBitCompassCalibrator.h"
#include "MicroBitDevice.h"
#include "MicroBitCompat.h"

#include "MicroBitBLEManager.h"
//#include "MicroBitStorage.h"
//#include "MicroBitLightSensor.h"



// Status flag values
#define DEVICE_INITIALIZED                    0x01

/**
 * Class definition for a MicroBit device.
 *
 * Represents the device as a whole, and includes member variables that represent various device drivers
 * used to control aspects of the micro:bit.
 */
namespace codal
{
    class MicroBit : public MicroBitDevice, public CodalComponent
    {
        private:

            /**
             * A listener to perform actions as a result of Message Bus reflection.
             *
             * In some cases we want to perform lazy instantiation of components, such as
             * the compass and the accelerometer, where we only want to add them to the idle
             * fiber when someone has the intention of using these components.
             */
            void onListenerRegisteredEvent(Event evt);

            // Pin ranges used for LED matrix display.

        public:

            codal::_mbed::Serial        serial;
            MessageBus                  messageBus;
            codal::_mbed::Timer         timer;
            MicroBitIO                  io;
            codal::_mbed::I2C           i2c;
            FXOS8700                    fxos;
            NRF52Pin*                   ledRowPins[5];
            NRF52Pin*                   ledColPins[6];
            const MatrixMap             ledMatrixMap;
            MicroBitDisplay             display;
            Button                      buttonA;
            Button                      buttonB;
            MultiButton                 buttonAB;
            MicroBitRadio               radio;
            MicroBitThermometer         thermometer;
            CoordinateSpace             coordinateSpace;
            FXOS8700Accelerometer       accelerometer;
            FXOS8700Magnetometer        compass;
            MicroBitCompassCalibrator   compassCalibrator;


            // Persistent key value store
            //MicroBitStorage           storage;

            #if CONFIG_ENABLED(MICROBIT_BLE_ENABLED)
            // Bluetooth related member variables.
            MicroBitBLEManager		  bleManager;
            BLEDevice                   *ble;
            #endif

            /**
             * Constructor.
             *
             * Create a representation of a Genuino Zero device, which includes member variables
             * that represent various device drivers used to control aspects of the board.
             */
            MicroBit();

            /**
             * Post constructor initialisation method.
             */
            int init();

            /**
             * Delay execution for the given amount of time.
             *
             * If the scheduler is running, this will deschedule the current fiber and perform
             * a power efficient, concurrent sleep operation.
             *
             * If the scheduler is disabled or we're running in an interrupt context, this
             * will revert to a busy wait.
             *
             * Alternatively: wait, wait_ms, wait_us can be used which will perform a blocking sleep
             * operation.
             *
             * @param milliseconds the amount of time, in ms, to wait for. This number cannot be negative.
             *
             * @return MICROBIT_OK on success, MICROBIT_INVALID_PARAMETER milliseconds is less than zero.
             *
             */
            void sleep(uint32_t milliseconds);

            /**
             * A periodic callback invoked by the fiber scheduler idle thread.
             * We use this for any low priority, background housekeeping.
             */
            virtual void idleCallback();

            /**
             * Determine the time since this MicroBit was last reset.
             *
             * @return The time since the last reset, in milliseconds.
             *
             * @note This will value overflow after 1.6 months.
             */
            //TODO: handle overflow case.
            unsigned long systemTime();
    };


    /**
     * Delay execution for the given amount of time.
     *
     * If the scheduler is running, this will deschedule the current fiber and perform
     * a power efficient, concurrent sleep operation.
     *
     * If the scheduler is disabled or we're running in an interrupt context, this
     * will revert to a busy wait.
     *
     * Alternatively: wait, wait_ms, wait_us can be used which will perform a blocking sleep
     * operation.
     *
     * @param milliseconds the amount of time, in ms, to wait for. This number cannot be negative.
     *
     * @return MICROBIT_OK on success, MICROBIT_INVALID_PARAMETER milliseconds is less than zero.
     *
     */
    inline void MicroBit::sleep(uint32_t milliseconds)
    {
        fiber_sleep(milliseconds);
    }

    /**
     * Determine the time since this MicroBit was last reset.
     *
     * @return The time since the last reset, in milliseconds.
     *
     * @note This will value overflow after 1.6 months.
     */
    inline unsigned long MicroBit::systemTime()
    {
        return system_timer_current_time();
    }
}

void microbit_dmesg_flush();

using namespace codal;

#endif
