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

#include "MicroBitConfig.h"
#include "CodalHeapAllocator.h"
#include "codal-core/inc/types/Event.h"
#include "CodalDevice.h"
#include "ErrorNo.h"
#include "NRFLowLevelTimer.h"
#include "CodalCompat.h"
#include "CodalComponent.h"
#include "CodalDmesg.h"
#include "ManagedType.h"
#include "ManagedString.h"
#include "NotifyEvents.h"

#include "Button.h"
#include "MultiButton.h"
#include "NRF52Pin.h"
#include "NRF52Serial.h"
#include "NRF52I2C.h"
#include "NRF52ADC.h"
#include "NRF52TouchSensor.h"

#include "MicroBitIO.h"
#include "MicroBitDisplay.h"
#include "CodalFiber.h"
#include "MessageBus.h"
#include "LSM303Accelerometer.h"
#include "LSM303Magnetometer.h"
#include "MicroBitRadio.h"
#include "MicroBitThermometer.h"
#include "MicroBitCompassCalibrator.h"
#include "MicroBitDevice.h"
#include "MicroBitCompat.h"
#include "MicroBitAccelerometer.h"
#include "MicroBitCompass.h"
#include "MicroBitPowerManager.h"
#include "NRF52FlashManager.h"
#include "MicroBitUSBFlashManager.h"
#include "MicroBitLog.h"
#include "MicroBitAudio.h"
#include "StreamNormalizer.h"
#include "LevelDetector.h"
#include "LevelDetectorSPL.h"
#include "SampleSource.h"
#include "PulseIn.h"
#include "neopixel.h"

#include "MESEvents.h"

#if CONFIG_ENABLED(DEVICE_BLE)
#include "MicroBitBLEManager.h"
#include "MicroBitLEDService.h"
#include "MicroBitAccelerometerService.h"
#include "MicroBitMagnetometerService.h"
#include "MicroBitButtonService.h"
#include "MicroBitIOPinService.h"
#include "MicroBitTemperatureService.h"
#include "MicroBitUARTService.h"
#include "MicroBitPartialFlashingService.h"
#include "MicroBitUtilityService.h"
#endif

#include "MicroBitStorage.h"


// Status flag values
#define DEVICE_INITIALIZED                    0x01

// Power on delay time (in milliseconds) applied after a hard power-on reset only.
#define KL27_POWER_ON_DELAY                    1000


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

            /**
             * A callback listener to disable default audio streaming to P0 if an event handler is registered on that pin.
             */
            void onP0ListenerRegisteredEvent(Event evt);

            /**
             * A callback listener to disable default audio streaming to P0 if an event handler is registered on that pin.
             */
            void onListenerRemovedEvent(Event evt);

            // Pin ranges used for LED matrix display.

        public:

#if CONFIG_ENABLED(DEVICE_BLE)
            // Bluetooth related member variables.
            // Initialize buttonless SVCI bootloader interface before interrupts are enabled
            MicroBitBLEManager          bleManager;
            BLEDevice                  *ble;
#endif

            NRFLowLevelTimer            systemTimer;
            NRFLowLevelTimer            adcTimer;
            NRFLowLevelTimer            capTouchTimer;
            Timer                       timer;
            MessageBus                  messageBus;
            NRF52ADC                    adc;
            NRF52TouchSensor            touchSensor;
            MicroBitIO                  io;
            NRF52Serial                 serial;
            MicroBitI2C                 _i2c;                   //Internal I2C for motion sensors
            MicroBitI2C                 i2c;                    //External I2C for edge connector
            MicroBitPowerManager        power;
            MicroBitUSBFlashManager     flash;
            NRF52FlashManager           internalFlash; 
            MicroBitStorage             storage;                // Persistent key value store
            NRF52Pin*                   ledRowPins[5];
            NRF52Pin*                   ledColPins[5];
            const MatrixMap             ledMatrixMap;
            MicroBitDisplay             display;
            Button                      buttonA;
            Button                      buttonB;
            MultiButton                 buttonAB;
            TouchButton                 logo;
            MicroBitRadio               radio;
            MicroBitThermometer         thermometer;
            Accelerometer&              accelerometer;
            Compass&                    compass;
            MicroBitCompassCalibrator   compassCalibrator;
            MicroBitAudio               audio;
            MicroBitLog                 log;


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
              * Return the serial number of this device.
              *
              * @return A ManagedString representing the serial number of this device.
              *
              * @code
              * ManagedString serialNumber = uBit.getSerial();
              * @endcode
              */
            static ManagedString getSerial();

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
             * @code
             * uBit.sleep(20); //sleep for 20ms
             * @endcode
             *
             * @note This operation is currently limited by the rate of the system timer, therefore
             *       the granularity of the sleep operation is limited to 4 ms, unless the rate of
             *       the system timer is modified.
             */
            void sleep(uint32_t milliseconds);

            /**
             * Perfom scheduler idle
             */
            virtual void schedulerIdle() override;

            /**
             * A periodic callback invoked by the fiber scheduler idle thread.
             * We use this for any low priority, background housekeeping.
             */
            virtual void idleCallback() override;

            /**
            * Periodic callback from Device system timer.
            *
            */
            virtual void periodicCallback() override;

            /**
             * Determine the time since this MicroBit was last reset.
             *
             * @return The time since the last reset, in milliseconds.
             *
             * @note This will value overflow after 1.6 months.
             */
            //TODO: handle overflow case.
            unsigned long systemTime();

            /**
             * Determines if any persistent storage needs to be erased following the reprogramming
             * of the micro:bit.
             *
             * @param forceErase Force an erase of user data, even if we have not detected a reflash event.
             */
            void eraseUserStorage(bool forceErase = false);
    };


    /**
      * Return the serial number of this device.
      *
      * @return A ManagedString representing the serial number of this device.
      *
      * @code
      * ManagedString serialNumber = uBit.getSerial();
      * @endcode
      */
    inline ManagedString MicroBit::getSerial()
    {
        uint64_t n = target_get_serial();
        int d = 1000000000;
        int n1 = n % d; n /= d;
        int n2 = n % d; n /= d;
        int n3 = n % d; n /= d;
        ManagedString s1(n1);
        ManagedString s2(n2);
        ManagedString s3(n3);
        ManagedString s = s3 + s2 + s1;
        return s;
        
    }

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
     * @code
     * uBit.sleep(20); //sleep for 20ms
     * @endcode
     *
     * @note This operation is currently limited by the rate of the system timer, therefore
     *       the granularity of the sleep operation is limited to 4 ms, unless the rate of
     *       the system timer is modified.
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
uint32_t *microbit_top_of_flash();

#if CONFIG_ENABLED(CODAL_USE_GLOBAL_NAMESPACE)
using namespace codal;
#endif

#define DEVICE_ID_MICROPHONE 3001

#define SOUND_MIRROR_EXTENSION uBit.io.speaker

#define MIC_DEVICE NRF52ADCChannel*
#define MIC_INIT \
    : microphone(uBit.audio.mic) \
    , level(* uBit.audio.levelSPL)

#define MIC_ENABLE //uBit.audio.deactivateLevelSPL(); //uBit.io.runmic.setDigitalValue(1); uBit.io.runmic.setHighDrive(true); microphone->setGain(7,0)

#endif
