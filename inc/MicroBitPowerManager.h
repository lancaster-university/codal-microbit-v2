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

#ifndef MICROBIT_POWER_MANAGER_H
#define MICROBIT_POWER_MANAGER_H

#include "MicroBitConfig.h"
#include "MicroBitCompat.h"
#include "MicroBitIO.h"
#include "codal-core/inc/core/CodalComponent.h"
#include "codal-core/inc/driver-models/I2C.h"
#include "codal-core/inc/driver-models/Pin.h"


// Constants for USB Interface Power Management Protocol

//
// I2C Address of the USB Interface Chip (Control interface)
//
#define MICROBIT_UIPM_I2C_ADDRESS	                (0x70 << 1)

//
// General constants 
//
#define MICROBIT_UIPM_MAX_BUFFER_SIZE               12
#define MICROBIT_UIPM_MAX_RETRIES                   20
#define MICROBIT_USB_INTERFACE_IRQ_THRESHOLD        30

//
// Command codes for the USB Interface Chip
//
#define MICROBIT_UIPM_COMMAND_READ_REQUEST           0x10
#define MICROBIT_UIPM_COMMAND_READ_RESPONSE          0x11
#define MICROBIT_UIPM_COMMAND_WRITE_REQUEST          0x12
#define MICROBIT_UIPM_COMMAND_WRITE_RESPONSE         0x13
#define MICROBIT_UIPM_COMMAND_ERROR_RESPONSE         0x20

//
// Queryable/Settable properties of the USB Interface Chip
//

#define MICROBIT_UIPM_PROPERTY_BOARD_REVISION       0x01
#define MICROBIT_UIPM_PROPERTY_I2C_VERSION          0x02
#define MICROBIT_UIPM_PROPERTY_DAPLINK_VERSION      0x03
#define MICROBIT_UIPM_PROPERTY_POWER_SOURCE         0x04
#define MICROBIT_UIPM_PROPERTY_POWER_CONSUMPTION    0x05
#define MICROBIT_UIPM_PROPERTY_USB_STATE            0x06
#define MICROBIT_UIPM_PROPERTY_KL27_POWER_MODE      0x07
#define MICROBIT_UIPM_PROPERTY_KL27_POWER_LED_STATE 0x08
#define MICROBIT_UIPM_PROPERTY_KL27_USER_EVENT      0x09

//
// KL27 User event sources
//
#define MICROBIT_UIPM_EVENT_WAKE_RESET              0x01
#define MICROBIT_UIPM_EVENT_WAKE_USB_INSERTION      0x02
#define MICROBIT_UIPM_EVENT_RESET_LONG_PRESS        0x03

//
// USB Interface Power Management Protocol error codes
//
#define MICROBIT_UIPM_SUCCESS	          0x30
#define MICROBIT_UIPM_INCOMPLETE_CMD	  0x31
#define MICROBIT_UIPM_UNKNOWN_CMD	      0x32
#define MICROBIT_UIPM_FORBIDDEN 	      0x33
#define MICROBIT_UIPM_NOT_RECOGNISED    0x34
#define MICROBIT_UIPM_BAD_LENGTH        0x35
#define MICROBIT_UIPM_READ_FORBIDDEN    0x36                    // Why not just FORBIDDEN?
#define MICROBIT_UIPM_WRITE_FORBIDDEN   0x37                    // Why not just FORBIDDEN?
#define MICROBIT_UIPM_WRITE_FAIL        0x38                    // Generalise to any FAIL?
#define MICROBIT_UIPM_BUSY              0x39



//
// Enumeration of device power modes
//
typedef enum {
    PWR_SOURCE_NONE = 0,
    PWR_USB_ONLY,
    PWR_BATT_ONLY,
    PWR_USB_AND_BATT
} MicroBitPowerSource;

//
// Enumeration of USB states
//
typedef enum {
    USB_DISCONNECTED = 0,
    USB_CONNECTING,
    USB_CONNECTED,
    USB_CHECK_CONNECTED,
    USB_CONFIGURED,
    USB_DISCONNECTING
} MicroBitUSBStatus;

//
// USB interface and device version information
//

// board versions:
// V2.0 (KL27 I/F):      0x9904
// V2.2 (NRF52833 I/F):  0x9905
// V2.2 (NRF52820 I/F):  0x9906

typedef struct {
    uint16_t        i2c;
    uint16_t        daplink;
    uint16_t        board;
} MicroBitVersion;

typedef struct {
    uint32_t batteryMicroVolts;
    uint32_t vinMicroVolts;
    float estimatedPowerConsumption;
} MicroBitPowerData;

//
// USB Interface Chip Power States
//
#define MICROBIT_USB_INTERFACE_POWER_MODE_VLPS    0x06          // Light Sleep - wake up on I2C. First message ignored.  
#define MICROBIT_USB_INTERFACE_POWER_MODE_VLLS0   0x08          // Deep Sleep - wake on reset + .... ??? Do we need this?

//
// Component Status flags
//
#define MICROBIT_USB_INTERFACE_AWAITING_RESPONSE   0x01
#define MICROBIT_USB_INTERFACE_VERSION_LOADED      0x02
#define MICROBIT_USB_INTERFACE_ALWAYS_NOP          0x04
#define MICROBIT_USB_INTERFACE_BUSY_FLAG_SUPPORTED 0x20

//
// Minimum deep sleep time (milliseconds)
//
#ifndef CONFIG_MINIMUM_DEEP_SLEEP_TIME
#define CONFIG_MINIMUM_DEEP_SLEEP_TIME  100
#endif

//
// Minimum time between power up and power down (milliseconds)
//
#ifndef CONFIG_MINIMUM_POWER_ON_TIME
#define CONFIG_MINIMUM_POWER_ON_TIME  500
#endif


/**
 * Class definition for MicroBitPowerManager.
 */
class MicroBitPowerManager : public CodalComponent
{
    public:
    
        MicroBitVersion         version;                            // Hardware versioning information
        MicroBitPowerSource     powerSource;                        // Last known power source
        //uint32_t                powerConsumption;                   // Last known power consumption (units?)
        MicroBitPowerData       powerData;                          //
        MicroBitUSBStatus       usbStatus;                          // Last known USB status
        MicroBitI2C             &i2cBus;                            // The I2C bus to use to communicate with USB interface chip
        MicroBitIO              &io;                                // Pins used by this device
        NRFLowLevelTimer        *sysTimer;                          // The system timer. 

        /**
         * Constructor.
         * Create a software abstraction of a power manager.
         *
         * @param i2c the I2C bus to use to communicate with the micro:bit USB interface chip
         * @param ioPins the IO pins in use on this device.
         * @param id the unique EventModel id of this component. Defaults to: MICROBIT_ID_POWER_MANAGER
         * @param systemTimer the system timer.
         *
         */
        MicroBitPowerManager(MicroBitI2C &i2c, MicroBitIO &ioPins, NRFLowLevelTimer &systemTimer, uint16_t id = MICROBIT_ID_POWER_MANAGER);

        /**
         * Attempts to determine the power source currently in use on this micro:bit.
         * 
         * @note This will query the USB interface chip via I2C, and wait for completion.
         * 
         * @return the current power source used by this micro:bit
         */
        MicroBitPowerSource getPowerSource();

        /**
         * Attempts to determine the instantaneous power consumption of this micro:bit.
         * 
         * @note This will query the USB interface chip via I2C, and wait for completion.
         * 
         * @warning The value returned here is actually the <code>estimatedPowerConsumption</code> in the powerData member variable, @see getPowerData() for information.
         * 
         * @deprecated To reduce the number of calls to the KL27, please use getPowerData() @see getPowerData()
         * @return the current estimated power consumption of this micro:bit
         */
        uint32_t getPowerConsumption() __attribute__ ((deprecated("Use getPowerData() instead")));

        /**
         * Requests the current power data from the interface chip, and calculates some approximate, but potentially useful values.
         * 
         * @note This will query the USB interface chip via I2C, and wait for completion.
         * 
         * @warning Values in micro-volts in the returned structure are measured, whereas <code>estimatedPowerConsumption</code> is an approximation and subject to change.
         * 
         * @return MicroBitPowerData The data from the interface chip, plus calculated fields.
         */
        MicroBitPowerData getPowerData();

        /**
         * Attempts to determine the USB interface chip version in use on this micro:bit.
         * 
         * @note This will query the USB interface chip via I2C if necessary.
         * 
         * @return MicroBitUSBVersion information.
         */
        MicroBitVersion getVersion();

        /**
         * Attempts to determine the status of the USB interface on this micro:bit.
         * 
         * @note This will query the USB interface chip via I2C, and wait for completion.
         * 
         * @return the current status of the USB interface on this micro:bit
         */
        MicroBitUSBStatus getUSBStatus();
        
        /**
         * Attempts to issue a control packet to the USB interface chip.
         * @param packet The data to send
         * @return MICROBIT_OK on success, or an I2C related error code on failure.
         */
        int sendUIPMPacket(ManagedBuffer packet);

        /**
         * Attempts to read a packet from the USB interface chip, either as a response to a
         * prior request, or following an interrupt request.
         * 
         * @return A buffer containing the complete response.
         */
        ManagedBuffer recvUIPMPacket();

        /**
         * Awaits a response to a previous requests to the USB interface chip.
         * Up to MICROBIT_UIPM_MAX_RETRIES attempts will be made at ~1ms intervals.
         * 
         * @return A buffer containing the complete response.
         */
        ManagedBuffer awaitUIPMPacket();

        /**
         * Attempts to issue a control packet to the USB interface chip.
         * @param packet The data to send
         * @return MICROBIT_OK on success 
         */
        ManagedBuffer writeProperty(ManagedBuffer request, bool ack = true);

        /**
         * Attempts to issue a control packet to the USB interface chip.
         * @param packet The data to send
         * @return A response or error packet 
         */
        ManagedBuffer readProperty(int property);

        /**
         * Perform a NULL opertion I2C transcation wit the interface chip.
         * This is used to awken the KL27 interface chip from light sleep, 
         * as a work around for silicon errata in the KL27.
         */
        void nop();

        /**
         * Powers down the CPU and USB interface and enters STANDBY state. All user code and peripherals will cease operation. 
         * Device can subsequently be awoken only via a RESET. User program state will be lost and will restart
         * from main().
         */
        void off();

        /**
         * Service any IRQ requests raised by the USB interface chip.
         */
        void readInterfaceRequest();

        /**
         * A periodic callback invoked by the fiber scheduler idle thread.
         * Service any IRQ requests raised by the USB interface chip.
         */
        virtual void idleCallback() override;

        /**
         * Allows a subsystem to indicate that it is actively waiting for a I2C response from the KL27
         * (e.g. the USBFlashManager). If set, the PowerManager will defer polling of the KL27 control interface
         * if an interrupt is asserted.
         *
         * @param awaiting true if a subsystem is awaiting a packet from the KL27, false otherwise.
         */
        void awaitingPacket(bool awaiting);

        /**
         * Destructor.
         */
        ~MicroBitPowerManager();

        /**
         * Powers down the CPU and USB interface and instructs peripherals to enter an inoperative low power state. However, all
         * program state is preserved. CPU will deepsleep for the given period of time, before returning to normal
         * operation.
         * 
         * @note ALL peripherals will be shutdown in this period. If you wish to keep peripherals active,
         * simply use uBit.sleep();
         *
         * The current fiber is blocked immediately.
         * Power down occurs when the scheduler is next idle, unless cancelled by wake up events before then.
         *
         * Wake up is triggered at the next Timer event created with the CODAL_TIMER_EVENT_FLAGS_WAKEUP flag
         * or by externally configured sources, for example, pin->wakeOnActive(true).
         */
        void deepSleep();

        /**
          * Powers down the CPU and USB interface and instructs peripherals to enter an inoperative low power state. However, all
          * program state is preserved. CPU will deepsleep for the given period of time, before returning to normal
          * operation.
          * 
          * @note ALL peripherals will be shutdown in this period. If you wish to keep peripherals active,
          * simply use uBit.sleep();
          *
          * The current fiber is blocked immediately.
          * Sleep occurs when the scheduler is next idle, unless cancelled by wake up events before then.
          *
          * If deep sleep is disturbed within the requested time interval, the remainder will be awake.
          * 
          * If the requested time interval is less than CONFIG_MINIMUM_DEEP_SLEEP_TIME, or a wake-up timer
          * cannot be allocated, the sleep will be a simple uBit.sleep() without powering down.
          *
          * @param milliSeconds The period of time to sleep, in milliseconds.
          */
        void deepSleep(uint32_t milliSeconds);

        bool deepSleep(uint32_t milliSeconds, bool interruptable );

        /**
         * Powers down the CPU and USB interface and instructs peripherals to enter an inoperative low power state. However, all
         * program state is preserved. CPU will deepsleep until the next codal::Timer event or other wake up source event, before returning to normal
         * operation.
         *
         * @note ALL peripherals will be shutdown in this period. If you wish to keep peripherals active,
         * simply use uBit.sleep();
         *
         * The function returns immediately.
         * Power down occurs when the scheduler is next idle, unless cancelled by wake up events before then.
         */
        void deepSleepAsync();

        /**
         * Enable power down during deepSleep
         * The default is enabled.
         */
        void powerDownEnable();

        /**
         * Disable power down during deepSleep
         * The default is enabled.
        */
        void powerDownDisable();

        /**
          * Determine if power down during deepSleep is enabled
        */
        bool powerDownIsEnabled();

        /**
         * For library use.
         * Determine if deep sleep has been requested.  
         * @return true if deep sleep has been requested
         */
        bool waitingForDeepSleep();

        /**
         * For library use.
         * Determine if the program is ready for deep sleep. 
         * @return true if deep sleep should be entered now
         */
        bool readyForDeepSleep();

        /**
         * For library use.
         * Start previosly requested deep sleep.
         * @return DEVICE_OK if deep sleep occurred, or DEVICE_INVALID_STATE if no usable wake up source is available
         */
        int startDeepSleep();

        /**
         * For library use.
         * Cancel previosly requested deep sleep.
         */
        void cancelDeepSleep();

        private:

        /**
          * Listener
          */
        void listener(Event evt);
        void listen();
        void ignore();
        
        /**
         * Prepare for deep sleep
         */
        void prepareDeepSleep();

        /**
         * Wait on the lock;
         */
        void deepSleepWait();

        /**
         * Update the status of the power LED
         * 
         * @param doSleep Set to true to preapre for sleep, false to prepare to reawaken.
         */
        void setPowerLED(bool doSleep);
        
        static volatile uint16_t timer_irq_channels;
        static void deepSleepTimerIRQ(uint16_t chan);

        FiberLock               deepSleepLock;
        int                     powerDownDisableCount;
        CODAL_TIMESTAMP         powerUpTime;
        uint16_t                eventValue;

        /**
         * Check if there are suitable wake-up sources for deep sleep
         *
         * @param wakeOnTime    Set to true to wake up at time wakeUpTime
         * @param wakeUpTime    Time to wake up.
         * @param wakeUpSources Set to true to use external wake up sources configured by e.g. pin->setAwakeOnActive(true)
         * @param wakeUpPin     Pin to wake up. Ignored if wakeUpSources == true.
         *
         * @return DEVICE_OK if OK to sleep 
         */
        int canDeepSleep( bool wakeOnTime, CODAL_TIMESTAMP wakeUpTime, bool wakeUpSources, NRF52Pin *wakeUpPin);

        /**
         * Enter deep sleep, unless no suitable wake-up sources have been configured. 
         *
         * @return DEVICE_OK if deep sleep occurred, or DEVICE_INVALID_STATE if no usable wake up source is available
         */
        int simpleDeepSleep();

        /**
         * Enter deep sleep, unless no suitable wake-up sources have been configured. 
         *
         * @param wakeOnTime    Set to true to wake up at time wakeUpTime
         * @param wakeUpTime    Time to trigger wake up. Ignored if wakeOnTime == false;
         * @param wakeUpSources Set to true to use wake up sources externally configured by, for example, pin->wakeOnActive(true)
         * @param wakeUpPin     Pin to trigger wake up. Ignored if wakeUpSources == true.
         *
         * @return DEVICE_OK if deep sleep occurred, or DEVICE_INVALID_STATE if no usable wake up source is available
         */
        int simpleDeepSleep( bool wakeOnTime, CODAL_TIMESTAMP wakeUpTime, bool wakeUpSources, NRF52Pin *wakeUpPin);
};
#endif
