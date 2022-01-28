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

#include "MicroBitPowerManager.h"
#include "MicroBit.h"

static const uint8_t UIPM_I2C_NOP[3] = {0,0,0};

static const KeyValueTableEntry uipmPropertyLengthData[] = {
    {MICROBIT_UIPM_PROPERTY_BOARD_REVISION, 2},
    {MICROBIT_UIPM_PROPERTY_I2C_VERSION,2},
    {MICROBIT_UIPM_PROPERTY_DAPLINK_VERSION, 2},
    {MICROBIT_UIPM_PROPERTY_POWER_SOURCE, 1},
    {MICROBIT_UIPM_PROPERTY_POWER_CONSUMPTION, 4},
    {MICROBIT_UIPM_PROPERTY_USB_STATE, 1},
    {MICROBIT_UIPM_PROPERTY_KL27_POWER_MODE, 1},
    {MICROBIT_UIPM_PROPERTY_KL27_POWER_LED_STATE, 1}
};
CREATE_KEY_VALUE_TABLE(uipmPropertyLengths, uipmPropertyLengthData);

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
MicroBitPowerManager::MicroBitPowerManager(MicroBitI2C &i2c, MicroBitIO &ioPins, NRFLowLevelTimer &systemTimer, uint16_t id) :
    i2cBus(i2c),
    io(ioPins),
    sysTimer(&systemTimer), 
    powerDownDisableCount(0),
    powerUpTime(0),
    eventValue(0)
{
    this->id = id;

    // Indicate we'd like to receive periodic callbacks both in idle and interrupt context.
    // Also, be pessimistic about the interface chip in use, until we obtain version information.
    status |= (DEVICE_COMPONENT_STATUS_IDLE_TICK | MICROBIT_USB_INTERFACE_ALWAYS_NOP);
}

/**
 * Attempts to determine the power source currently in use on this micro:bit.
 * note: This will query the USB interface chip via I2C, and wait for completion.
 * 
 * @return the current power source used by this micro:bit
 */
MicroBitPowerSource MicroBitPowerManager::getPowerSource()
{
    ManagedBuffer b;
    b = readProperty(MICROBIT_UIPM_PROPERTY_POWER_SOURCE);

    powerSource = (MicroBitPowerSource)b[3];

    return powerSource;
}

/**
 * Attempts to determine the USB interface chip version in use on this micro:bit.
 * note: This will query the USB interface chip via I2C if necessary.
 * 
 * @return MicroBitUSBVersion information.
 */
MicroBitVersion MicroBitPowerManager::getVersion()
{
    if (!(status & MICROBIT_USB_INTERFACE_VERSION_LOADED))
    {
        ManagedBuffer b;
        
        // Read Board Revision ID
        b = readProperty(MICROBIT_UIPM_PROPERTY_BOARD_REVISION);
        memcpy(&version.board, &b[3], 2);

        // Read I2C protocol version 
        b = readProperty(MICROBIT_UIPM_PROPERTY_I2C_VERSION);
        memcpy(&version.i2c, &b[3], 2);

        // Read DAPLink version
        b = readProperty(MICROBIT_UIPM_PROPERTY_DAPLINK_VERSION);
        memcpy(&version.daplink, &b[3], 2);

        // Version data in non-volatile, so cache it for later.
        status |= MICROBIT_USB_INTERFACE_VERSION_LOADED;

        // Optimise our behaviour for the version of interface chip in use.
        switch(version.board)
        {
            // V2.00 KL27
            case 39172:
                status |= MICROBIT_USB_INTERFACE_ALWAYS_NOP;
                break;

            default:
                status &= ~MICROBIT_USB_INTERFACE_ALWAYS_NOP;
        }
    }

    return version;
}

/**
 * Attempts to determine the status of the USB interface on this micro:bit.
 * note: This will query the USB interface chip via I2C, and wait for completion.
 * 
 * @return the current status of the USB interface on this micro:bit
 */
MicroBitUSBStatus MicroBitPowerManager::getUSBStatus()
{
    ManagedBuffer b;
    b = readProperty(MICROBIT_UIPM_PROPERTY_USB_STATE);

    usbStatus = (MicroBitUSBStatus)b[3];
    return usbStatus;
}

/**
 * Attempts to determine the instantaneous power consumption of this micro:bit.
 * note: This will query the USB interface chip via I2C, and wait for completion.
 * 
 * @return the current power consumption of this micro:bit
 */
uint32_t MicroBitPowerManager::getPowerConsumption()
{
    ManagedBuffer b;
    b = readProperty(MICROBIT_UIPM_PROPERTY_POWER_CONSUMPTION);

    memcpy(&powerConsumption, &b[3], 4);
    return powerConsumption;
}

/**
 * Perform a NULL opertion I2C transaction with the interface chip if needed.
 * This is used to awken the KL27 interface chip from light sleep, 
 * as a work around for silicon errata in the KL27.
 */
void MicroBitPowerManager::nop()
{
    // Perform a throw away zero byte write operation to the interface chip if needed.
    // KL27 based implementations need this to wake from deep sleep.
    if (status & MICROBIT_USB_INTERFACE_ALWAYS_NOP)
    {
        uint8_t unused;
        i2cBus.write(MICROBIT_UIPM_I2C_ADDRESS, &unused, 0, false);
    }
}

/**
 * Attempts to issue a control packet to the USB interface chip.
 * @param packet The data to send
 * @return MICROBIT_OK on success, or an I2C related error code on failure.
 */
int MicroBitPowerManager::sendUIPMPacket(ManagedBuffer packet)
{
    nop();
    return i2cBus.write(MICROBIT_UIPM_I2C_ADDRESS, &packet[0], packet.length(), false);
}

/**
 * Attempts to read a packet from the USB interface chip, either as a response to a
 * prior request, or following an interrupt request.
 * 
 * @return A buffer containing the complete response.
 */
ManagedBuffer MicroBitPowerManager::recvUIPMPacket()
{

    if(io.irq1.isActive())
    {
        ManagedBuffer b(MICROBIT_UIPM_MAX_BUFFER_SIZE);

        nop();  
        if (i2cBus.read(MICROBIT_UIPM_I2C_ADDRESS, &b[0], MICROBIT_UIPM_MAX_BUFFER_SIZE, false) == MICROBIT_OK)
            return b;
    }

    return ManagedBuffer();
}

/**
 * Awaits a response to a previous requests to the USB interface chip.
 * Up to MICROBIT_UIPM_MAX_RETRIES attempts will be made at ~1ms intervals.
 * 
 * @return A buffer containing the complete response.
 */
ManagedBuffer MicroBitPowerManager::awaitUIPMPacket()
{
    ManagedBuffer response;
    int attempts = 0;

    awaitingPacket(true);

    // Wait for a response, signalled (possibly!) by a LOW on the combined irq line
    // Retry until we get a valid response or we time out.
    while(attempts++ < MICROBIT_UIPM_MAX_RETRIES)
    {
        target_wait(1);

        // Try to read a response from the KL27
        response = recvUIPMPacket();
        
        // If we receive an INCOMPLETE response, then the KL27 is still working on our request, so wait a little longer and try again.
        // Similarly, if the I2C transaction fails, retry.
        if (response.length() == 0 || (response[0] == MICROBIT_UIPM_COMMAND_ERROR_RESPONSE && response[1] == MICROBIT_UIPM_INCOMPLETE_CMD))
            continue;

        // Sanitize the length of the packet to meet specification and return it.
        response.truncate((response[0] == MICROBIT_UIPM_COMMAND_ERROR_RESPONSE || response[0] == MICROBIT_UIPM_COMMAND_WRITE_RESPONSE) ? 2 : 3 + uipmPropertyLengths.get(response[1]));
        awaitingPacket(false);

        return response;
    }

    // If we time out, return a generic write error to the caller
    DMESG("UIPM: Await timeout");
    ManagedBuffer error(2);
    error[0] = MICROBIT_UIPM_COMMAND_ERROR_RESPONSE;
    error[1] = MICROBIT_UIPM_WRITE_FAIL;
    awaitingPacket(false);

    return error;
}


/**
 * Attempts to issue a control packet to the USB interface chip.
 * @param packet The data to send
 * @return MICROBIT_OK on success 
 */
ManagedBuffer MicroBitPowerManager::writeProperty(ManagedBuffer request, bool ack)
{
    ManagedBuffer response;

    if (sendUIPMPacket(request) == MICROBIT_OK && ack)
    {
        response = awaitUIPMPacket();
    }

    return response;
}

/**
 * Attempts to issue a control packet to the USB interface chip.
 * @param packet The data to send
 * @return A response or error packet 
 */
ManagedBuffer MicroBitPowerManager::readProperty(int property)
{
    ManagedBuffer request(2);
    ManagedBuffer response;

    request[0] = MICROBIT_UIPM_COMMAND_READ_REQUEST;
    request[1] = property;

    if (sendUIPMPacket(request) == MICROBIT_OK)
        response = awaitUIPMPacket();
    
    return response;
}
        
/**
 * Powers down the CPU and USB interface and enters OFF state. All user code and peripherals will cease operation. 
 * Device can subsequently be awoken only via a RESET. User program state will be lost and will restart
 * from main().
 */
void MicroBitPowerManager::off()
{
    setPowerLED( true /*doSleep*/);

    // Update peripheral drivers
    CodalComponent::deepSleepAll( deepSleepCallbackBegin, NULL);

    // Instruct the KL27 interface chip to go into deep sleep.
    ManagedBuffer sleepCommand(4);
    sleepCommand[0] = MICROBIT_UIPM_COMMAND_WRITE_REQUEST;
    sleepCommand[1] = MICROBIT_UIPM_PROPERTY_KL27_POWER_MODE;
    sleepCommand[2] = 1;
    sleepCommand[3] = MICROBIT_USB_INTERFACE_POWER_MODE_VLLS0;
    writeProperty(sleepCommand, true);

    // Wait a little while to ensure all hardware and peripherals have reacted to the change of power mode.
    target_wait(10);

    // Configure combined IRQ line to DETECT HI->LO transitions and reset the NRF52.
    // When sleeping, this allows us to reset on USB insertion, as raised by the KL27 interface chip.
    io.irq1.setDetect(GPIO_PIN_CNF_SENSE_Low);

    // Enter System Off state.
    NRF_POWER->SYSTEMOFF = 1;
}

/**
 * A periodic callback invoked by the fiber scheduler idle thread.
 * Service any IRQ requests raised by the USB interface chip.
 */
void MicroBitPowerManager::idleCallback()
{
    static int activeCount = 0;

    // Do nothing if there is a transaction in progress.
    if (status & MICROBIT_USB_INTERFACE_AWAITING_RESPONSE || !io.irq1.isActive())
    {
        activeCount = 0;
        return;
    }

    // Ensure the line has been held low for a little before services, as other sensors
    // are much more likley to be the source of the IRQ.
    if (activeCount++ < MICROBIT_USB_INTERFACE_IRQ_THRESHOLD)
        return;

    // The line has been held active beyond our threshold.
    // Reset our counter, and see if the USB interface chip has any data ready for us.
    activeCount  = 0;

    // Determine if the KL27 is trying to indicate an event
    ManagedBuffer response;
    response = recvUIPMPacket();

    if (response.length() > 0)
    {    
        // We have a valid frame.
        if(response[0] == MICROBIT_UIPM_COMMAND_READ_RESPONSE && response[1] == MICROBIT_UIPM_PROPERTY_KL27_USER_EVENT && response[2] == 1)
        {
            // The frame is for us - process the event.
            switch (response[3])
            {
                case MICROBIT_UIPM_EVENT_WAKE_RESET:
                    DMESG("WAKE FROM RESET BUTTON");
                    break;

                case MICROBIT_UIPM_EVENT_WAKE_USB_INSERTION:
                    DMESG("WAKE FROM USB");
                    break;

                case MICROBIT_UIPM_EVENT_RESET_LONG_PRESS:
                    DMESG("LONG RESET BUTTON PRESS");
                    off();
                    break;

                default:
                    DMESG("UNKNOWN KL27 EVENT CODE [%d]", response[2]);
            }
        }
        else
        {
            // The frame is not for us - forward the event to a Flash Manager if it has been registered
            DMESG("UIPM: RECEIVED UNKNWON FRAME");
        }
        
    }
}

/**
 * Update the status of the power LED
 * 
 * @param doSleep Set to true to preapre for sleep, false to prepare to reawaken.
 */
void MicroBitPowerManager::setPowerLED(bool doSleep)
{
    // Instruct the KL27 interface chip to update its LED status the power LED
    ManagedBuffer sleepCommand(4);
    sleepCommand[0] = MICROBIT_UIPM_COMMAND_WRITE_REQUEST;
    sleepCommand[1] = MICROBIT_UIPM_PROPERTY_KL27_POWER_LED_STATE;
    sleepCommand[2] = 1;
    sleepCommand[3] = doSleep ? 0 : 1;
    writeProperty(sleepCommand);
}

/**
 * Allows a subsystem to indicate that it is actively waiting for a I2C response from the KL27
 * (e.g. the USBFlashManager). If set, the PowerManager will defer polling of the KL27 control interface
 * if an interrupt is asserted.
 *
 * @param awaiting true if a subsystem is awaiting a packet from the KL27, false otherwise.
 */
void MicroBitPowerManager::awaitingPacket(bool awaiting)
{
    if (awaiting)
        status |= MICROBIT_USB_INTERFACE_AWAITING_RESPONSE;
    else
        status &= ~MICROBIT_USB_INTERFACE_AWAITING_RESPONSE;
}

/**
 * Destructor.
 */
MicroBitPowerManager::~MicroBitPowerManager()
{

}

////////////////////////////////////////////////////////////////
// deepSleep

/**
  * Powers down the CPU and USB interface and instructs peripherals to enter an inoperative low power state. However, all
  * program state is preserved. CPU will deepsleep for the given period of time, before returning to normal
  * operation.
  * 
  * note: ALL peripherals will be shutdown in this period. If you wish to keep peripherals active,
  * simply use uBit.sleep();
  *
  * The current fiber is blocked immediately. Sleep occurs when the scheduler is next idle.
  *
  * Wake up is triggered at the next Timer event created with the CODAL_TIMER_EVENT_FLAGS_WAKEUP flag
  * or by externally configured sources, for example, pin->wakeOnActive(true).
  */
void MicroBitPowerManager::deepSleep()
{
    // If the scheduler is not running, perform a simple deep sleep.
    if (!fiber_scheduler_running())
    {
        simpleDeepSleep();
        return;
    }

    CODAL_TIMESTAMP eventTime = 0;
    bool wakeOnTime = system_timer_deepsleep_wakeup_time( eventTime);
    int can = canDeepSleep( wakeOnTime, eventTime, true /*wakeUpSources*/, NULL /*wakeUpPin*/);
    if ( can == DEVICE_OK)
        deepSleepWait();
}

/**
  * Powers down the CPU and USB interface and instructs peripherals to enter an inoperative low power state. However, all
  * program state is preserved. CPU will deepsleep for the given period of time, before returning to normal
  * operation.
  * 
  * note: ALL peripherals will be shutdown in this period. If you wish to keep peripherals active,
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
  * @param milliSeconds The period of time to sleep, in milliseconds (minimum CONFIG_MINIMUM_DEEP_SLEEP_TIME).
  */
void MicroBitPowerManager::deepSleep(uint32_t milliSeconds)
{
    if ( milliSeconds > CONFIG_MINIMUM_DEEP_SLEEP_TIME)
    {
        CODAL_TIMESTAMP timeEntry  = system_timer_current_time_us();
        CODAL_TIMESTAMP wakeUpTime = timeEntry + (CODAL_TIMESTAMP) 1000 * milliSeconds;

        // If the scheduler is not running, perform a simple deep sleep.
        if (!fiber_scheduler_running())
        {
            simpleDeepSleep( true /*wakeOnTime*/, wakeUpTime, true /*wakeUpSources*/, NULL /*wakeUpPin*/);
            return;
        }

        eventValue++;
        int result = system_timer_event_after( milliSeconds, id, eventValue, CODAL_TIMER_EVENT_FLAGS_WAKEUP);
        if ( result == DEVICE_OK)
        {
            deepSleepWait();

            CODAL_TIMESTAMP awake = system_timer_current_time_us();

            // Timed wake-up is usually < 20ms early here
            if ( wakeUpTime > awake + 1000)
            {
                // If another fiber triggers deep sleep
                // the wake-up timer is still in place
                fiber_sleep( (wakeUpTime - awake) / 1000);
            }

            system_timer_cancel_event( id, eventValue);
            return;
        }
    }

    // Block power down
    powerDownDisable();
    fiber_sleep( milliSeconds);
    powerDownEnable();
}

/**
  * Powers down the CPU and USB interface and instructs peripherals to enter an inoperative low power state. However, all
  * program state is preserved. CPU will deepsleep until the next codal::Timer event or other wake up source event, before returning to normal
  * operation.
  *
  * note: ALL peripherals will be shutdown in this period. If you wish to keep peripherals active,
  * simply use uBit.sleep();
  *
  * The function returns immediately. Sleep occurs when the scheduler is next idle.
  */
void MicroBitPowerManager::deepSleepAsync()
{
    // If the scheduler is not running, perform a simple deep sleep.
    if (!fiber_scheduler_running())
    {
        simpleDeepSleep();
        return;
    }

    CODAL_TIMESTAMP eventTime = 0;
    bool wakeOnTime = system_timer_deepsleep_wakeup_time( eventTime);
    int can = canDeepSleep( wakeOnTime, eventTime, true /*wakeUpSources*/, NULL /*wakeUpPin*/);
    if ( can == DEVICE_OK)
        prepareDeepSleep();
}


/**
  * Enable power down during deepSleep
  * The default is enabled.
  */
void MicroBitPowerManager::powerDownEnable()
{
    powerDownDisableCount--;
}

/**
  * Disable power down during deepSleep
  * The default is enabled.
*/
void MicroBitPowerManager::powerDownDisable()
{
    powerDownDisableCount++;
}

/**
  * Determine if power down during deepSleep is enabled
*/
bool MicroBitPowerManager::powerDownIsEnabled()
{
    return powerDownDisableCount <= 0;
}

////////////////////////////////////////////////////////////////
// deepSleep control

/**
  * Determine if deep sleep has been requested. For library use. 
  * @return true if deep sleep has been requested
  */
bool MicroBitPowerManager::waitingForDeepSleep()
{
    return fiber_scheduler_get_deepsleep_pending();
}

/**
  * Determine if the program is ready for deep sleep. For library use. 
  * @return true if deep sleep should be entered now
  */
bool MicroBitPowerManager::readyForDeepSleep()
{
    if ( system_timer_current_time() - powerUpTime < CONFIG_MINIMUM_POWER_ON_TIME)
        return false;

    return powerDownIsEnabled();
}

/**
  * Start previosly requested deep sleep
  * @return DEVICE_OK if deep sleep occurred, or DEVICE_INVALID_STATE if no usable wake up source is available
  */
int MicroBitPowerManager::startDeepSleep()
{
    fiber_scheduler_set_deepsleep_pending( false);
    ignore();
    int result = simpleDeepSleep();
    deepSleepLock.notifyAll();
    return result;
}

/**
  * Cancel previously requested deep sleep
  */
void MicroBitPowerManager::cancelDeepSleep()
{
    deepSleepLock.notifyAll();
    fiber_scheduler_set_deepsleep_pending( false);
    ignore();
}

////////////////////////////////////////////////////////////////
// deepSleep implementation

/**
  * Listener
  */
void MicroBitPowerManager::listener(Event evt)
{
    if ( evt.source == DEVICE_ID_NOTIFY)
    {
        if ( evt.value == POWER_EVT_CANCEL_DEEPSLEEP)
            cancelDeepSleep();
    }
}

void MicroBitPowerManager::listen()
{
    if (EventModel::defaultEventBus)
        EventModel::defaultEventBus->listen(DEVICE_ID_NOTIFY, POWER_EVT_CANCEL_DEEPSLEEP, this, &MicroBitPowerManager::listener, MESSAGE_BUS_LISTENER_IMMEDIATE);
}

void MicroBitPowerManager::ignore()
{
    if (EventModel::defaultEventBus)
        EventModel::defaultEventBus->ignore(DEVICE_ID_NOTIFY, POWER_EVT_CANCEL_DEEPSLEEP, this, &MicroBitPowerManager::listener);
}

/**
 * Prepare for deep sleep
 */
void MicroBitPowerManager::prepareDeepSleep()
{
    if ( !fiber_scheduler_get_deepsleep_pending())
    {
        fiber_scheduler_set_deepsleep_pending( true);
        listen();
        CodalComponent::deepSleepAll( deepSleepCallbackPrepare, NULL);
    }
}

/**
 * Wait on the lock
 */
void MicroBitPowerManager::deepSleepWait()
{
    if ( deepSleepLock.getWaitCount() == 0)
    {
        // This is OK, so long as we only use notifyAll() not notify()
        deepSleepLock.wait();
    }

    prepareDeepSleep();
    deepSleepLock.wait();
}


volatile uint16_t MicroBitPowerManager::timer_irq_channels;

void MicroBitPowerManager::deepSleepTimerIRQ(uint16_t chan)
{
    timer_irq_channels = chan;
}


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
int MicroBitPowerManager::canDeepSleep( bool wakeOnTime, CODAL_TIMESTAMP wakeUpTime, bool wakeUpSources, NRF52Pin *wakeUpPin)
{
    if ( sysTimer == NULL)
    {
        return DEVICE_NOT_SUPPORTED;
    }

    CODAL_TIMESTAMP timeEntry = system_timer_current_time_us();

    if ( wakeOnTime)
    {
        if ( wakeUpTime < timeEntry || wakeUpTime - timeEntry < CONFIG_MINIMUM_DEEP_SLEEP_TIME * 1000)
        {
            return DEVICE_INVALID_STATE;
        }
    }
    else
    {
        if ( wakeUpSources)
        {
            deepSleepCallbackData data;
            CodalComponent::deepSleepAll( deepSleepCallbackCountWakeUps, &data);
            if ( data.count == 0)
            {
                return DEVICE_INVALID_STATE;
            }
        }
        else
        {
            if ( wakeUpPin == NULL)
            {
                return DEVICE_INVALID_STATE;
            }
        }
    }

    return DEVICE_OK;
}

/**
  * Enter deep sleep, unless no suitable wake-up sources have been configured. 
  *
  * @return DEVICE_OK if deep sleep occurred, or DEVICE_INVALID_STATE if no usable wake up source is available
  */
int MicroBitPowerManager::simpleDeepSleep()
{
    CODAL_TIMESTAMP eventTime = 0;
    bool wakeOnTime = system_timer_deepsleep_wakeup_time( eventTime);
    return simpleDeepSleep( wakeOnTime, eventTime, true /*wakeUpSources*/, NULL /*wakeUpPin*/);
}

/**
 * Enter deep sleep, unless no suitable wake-up sources have been configured. 
 *
 * @param wakeOnTime    Set to true to wake up at time wakeUpTime
 * @param wakeUpTime    Time to wake up.
 * @param wakeUpSources Set to true to use external wake up sources configured by e.g. pin->setAwakeOnActive(true)
 * @param wakeUpPin     Pin to wake up. Ignored if wakeUpSources == true.
 */
int MicroBitPowerManager::simpleDeepSleep( bool wakeOnTime, CODAL_TIMESTAMP wakeUpTime, bool wakeUpSources, NRF52Pin *wakeUpPin)
{
    CODAL_TIMESTAMP timeEntry = system_timer_current_time_us();

    int can = canDeepSleep( wakeOnTime, wakeUpTime, wakeUpSources, wakeUpPin);
    if ( can != DEVICE_OK)
        return can;

    // Configure for sleep mode
    setPowerLED( true /*doSleep*/);

    // Update peripheral drivers
    CodalComponent::deepSleepAll( wakeUpSources ? deepSleepCallbackBeginWithWakeUps : deepSleepCallbackBegin, NULL);

    CODAL_TIMESTAMP tickStart;
    CODAL_TIMESTAMP timeStart = system_timer_deepsleep_begin( tickStart);

    int      channel      = 2;      //System timer uses period = 0, event = 1 and capture = 3
    uint32_t saveCompare  = sysTimer->timer->CC[channel];
    uint32_t saveIntenset = sysTimer->timer->INTENSET;

    sysTimer->timer->INTENCLR = sysTimer->timer->INTENSET;

    void (*sysTimerIRQ) (uint16_t channel_bitmsk) = sysTimer->timer_pointer;
    sysTimer->setIRQ( deepSleepTimerIRQ);
    timer_irq_channels = 0;

    uint32_t usPerTick   = 1;
    uint32_t ticksPerMS  = 1000;
    uint32_t ticksMax    = 0xFFFFFFFFul - ticksPerMS * 1000; // approx 71min

    if ( !wakeUpSources)
    {
        if ( wakeUpPin)
        {
            // Ensure the requested pin into digital input mode. 
            wakeUpPin->getDigitalValue();

            // Enable an interrupt on the requested pin or an interrupt from the KL27.
            wakeUpPin->setDetect(wakeUpPin->getPolarity() ? GPIO_PIN_CNF_SENSE_High : GPIO_PIN_CNF_SENSE_Low);
        }
    }

    // Enable wakeup from the the KL27 interrupt line.
    io.irq1.setDetect(GPIO_PIN_CNF_SENSE_Low);
    NVIC_EnableIRQ(GPIOTE_IRQn);

    sysTimer->setCompare( channel, tickStart);
    sysTimer->enableIRQ();

    uint32_t tick0 = tickStart;
    uint32_t tick1 = tick0;

    // assume wake-up needs at least same time as power down
    // it may need much longer in general
    uint64_t totalTicks = wakeUpTime - timeEntry
                        - (timeStart - timeEntry) * 2 / usPerTick
                        - 13;  // __WFI() latency

    uint64_t sleepTicks = 0;

    while ( true)
    {
        uint32_t remain;

        if ( wakeOnTime)
        {
            if ( sleepTicks >= totalTicks)
                break;
            uint64_t remain64 = totalTicks - sleepTicks;
            remain = remain64 > ticksMax ? ticksMax : remain64;
        }
        else
        {
          remain = ticksMax;
        }

        sysTimer->setCompare( channel, tick0 + remain);

        // Wait for an interrupt to occur. This will either be the requested transition,
        // or an asynchronous event from the KL27 interface chip.
        __WFI();

        tick1 = sysTimer->captureCounter();

        uint32_t ticks = tick1 - tick0;
        tick0 = tick1;

        sleepTicks += ticks;

#if CONFIG_ENABLED(CODAL_TIMER_32BIT)
        system_timer_current_time_us();
#endif

        if ( timer_irq_channels == 0)
        {
            break; // It must be another interrupt
        }

        timer_irq_channels = 0;
    }

    // Disable DETECT events 
    io.irq1.setDetect(GPIO_PIN_CNF_SENSE_Disabled);

    if ( !wakeUpSources)
    {
        if ( wakeUpPin)
            wakeUpPin->setDetect(GPIO_PIN_CNF_SENSE_Disabled);
    }

    // Restore timer state
    sysTimer->disableIRQ();
    sysTimer->timer->INTENCLR = sysTimer->timer->INTENSET;
    sysTimer->setIRQ(sysTimerIRQ);
    sysTimer->timer->CC[channel] = saveCompare;

#if CONFIG_ENABLED(CODAL_TIMER_32BIT)
    system_timer_deepsleep_end( 0, 0);
#else
    system_timer_deepsleep_end( tick1, sleepTicks * usPerTick);
    // Events queued between the return from __WFI() and now will have incorrect times 
#endif

    sysTimer->timer->INTENSET = saveIntenset;

    // Configure for running mode.
    CodalComponent::deepSleepAll( wakeUpSources ? deepSleepCallbackEndWithWakeUps : deepSleepCallbackEnd, NULL);

    setPowerLED(false /*doSleep*/);

    powerUpTime = system_timer_current_time();

    return DEVICE_OK;
}
