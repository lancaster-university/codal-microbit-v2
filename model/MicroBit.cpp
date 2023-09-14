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


#include "MicroBit.h"
#include "Timer.h"
#include "MicroBitDevice.h"
#include "MicroBitMemoryMap.h"
#include "CodalDmesg.h"

using namespace codal;

//
// Matrix layout model for LED Matrix
//
static const MatrixPoint ledMatrixPositions[5*5] =
{
    {0,0},{0,1},{0,2},{0,3},{0,4},
    {1,0},{1,1},{1,2},{1,3},{1,4},
    {2,0},{2,1},{2,2},{2,3},{2,4},
    {3,0},{3,1},{3,2},{3,3},{3,4},
    {4,0},{4,1},{4,2},{4,3},{4,4}
};

static const uint32_t reflash_status = 0xffffffff;

static volatile MicroBitNoInitMemoryRegion __attribute__ ((section (".noinit"))) microbit_no_init_memory_region;

/**
  * Constructor.
  *
  * Create a representation of a micro:bit device, which includes member variables
  * that represent various device drivers used to control aspects of the micro:bit.
  */
MicroBit::MicroBit() :

#if CONFIG_ENABLED(DEVICE_BLE)
    bleManager(),
    ble( &bleManager),
#endif

    systemTimer(NRF_TIMER1, TIMER1_IRQn),
    adcTimer(NRF_TIMER2, TIMER2_IRQn),

    capTouchTimer(NRF_TIMER3, TIMER3_IRQn),
    timer(systemTimer),
    messageBus(),
    adc(adcTimer, 91),
    touchSensor(capTouchTimer),
    io(adc, touchSensor),
    serial(io.usbTx, io.usbRx, NRF_UARTE0),
    _i2c(io.sda, io.scl),
    i2c(io.P20, io.P19),
    power(_i2c, io, systemTimer),
    flash(_i2c, io, power),
    internalFlash(MICROBIT_STORAGE_PAGE, 1, MICROBIT_CODEPAGESIZE),
    storage(internalFlash, 0),
    ledRowPins{&io.row1, &io.row2, &io.row3, &io.row4, &io.row5},
    ledColPins{&io.col1, &io.col2, &io.col3, &io.col4, &io.col5},
    ledMatrixMap{ 5, 5, 5, 5, (Pin**)ledRowPins, (Pin**)ledColPins, ledMatrixPositions},
    display(ledMatrixMap),
    buttonA(io.P5, DEVICE_ID_BUTTON_A, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW),
    buttonB(io.P11, DEVICE_ID_BUTTON_B, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW),
    buttonAB(DEVICE_ID_BUTTON_A, DEVICE_ID_BUTTON_B, DEVICE_ID_BUTTON_AB),
    logo(io.logo, touchSensor, CAPTOUCH_DEFAULT_CALIBRATION),
    radio(),
    thermometer(),
    accelerometer(MicroBitAccelerometer::autoDetect(_i2c)),
    compass(MicroBitCompass::autoDetect(_i2c)),
    compassCalibrator(compass, accelerometer, display, storage),
    audio(io.P0, io.speaker, adc, io.microphone, io.runmic),
    log(flash, power, serial)
{
    // Clear our status
    status = 0;

    /*
    // Ensure NFC pins are configured as GPIO. If not, update the non-volatile UICR.
    if (NRF_UICR->NFCPINS)
    {
        DMESG("RESET UICR\n");
        // Enable Flash Writes
        NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

        // Configure PINS for GPIO use.
        NRF_UICR->NFCPINS = 0;

        // Disable Flash Writes
        NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

        // Reset, so the changes can take effect.
        NVIC_SystemReset();
    }
    */

    // Configure serial port for debugging

    //SERIAL_TODO:
    // serial.set_flow_control(mbed::Serial::Disabled);
    //serial.baud(115200);

    // Enable the serial port as a deep sleep wake event. (Useful for MicroPython REPL, for example)
    #if CONFIG_ENABLED(MICROBIT_USB_SERIAL_WAKE)
        serial.status |= CODAL_SERIAL_STATUS_DEEPSLEEP;
    #endif

    // Add pullup resisitor to IRQ line (it's floating ACTIVE LO)
    io.irq1.getDigitalValue();
    io.irq1.setPull(PullMode::Up);
    io.irq1.setActiveLo();

    _i2c.setFrequency(400000);

    // Bring up our display pins as high drive.
    for (NRF52Pin *p : ledRowPins)
        p->setHighDrive(true);

    for (NRF52Pin *p : ledColPins)
        p->setHighDrive(true);

    // Bring up internal speaker as high drive.
    io.speaker.setHighDrive(true);
}

/**
  * Post constructor initialisation method.
  *
  * This call will initialised the scheduler, memory allocator and Bluetooth stack.
  *
  * This is required as the Bluetooth stack can't be brought up in a
  * static context i.e. in a constructor.
  *
  * @code
  * uBit.init();
  * @endcode
  *
  * @note This method must be called before user code utilises any functionality
  *       contained within the GenuinoZero class.
  */
int MicroBit::init()
{
    if (status & DEVICE_INITIALIZED)
        return DEVICE_NOT_SUPPORTED;

    status |= DEVICE_INITIALIZED;

    // On a hard reset, wait for the USB interface chip to come online.
    if(NRF_POWER->RESETREAS == 0)
    {
        microbit_no_init_memory_region.resetClickCount = 0;
        target_wait(KL27_POWER_ON_DELAY);
    }
    else
    {
        microbit_no_init_memory_region.resetClickCount++;
    }

#if CONFIG_ENABLED(DEVICE_BLE)
    // Ensure BLE bootloader settings are up to date.
    // n.b. this only performs a write operation if the settings stored in FLASH are out of date.
    MicroBitPartialFlashingService::validateBootloaderSettings();
#endif

    // Determine if we have been reprogrammed. If so, follow configured policy on erasing any persistent user data.
    eraseUserStorage();

    // Bring up fiber scheduler.
    scheduler_init(messageBus);

    for(int i = 0; i < DEVICE_COMPONENT_COUNT; i++)
    {
        if(CodalComponent::components[i])
            CodalComponent::components[i]->init();
    }

    // Seed our random number generator
    seedRandom();

    // Create an event handler to trap any handlers being created for I2C services.
    // We do this to enable initialisation of those services only when they're used,
    // which saves processor time, memeory and battery life.
    messageBus.listen(DEVICE_ID_MESSAGE_BUS_LISTENER, DEVICE_EVT_ANY, this, &MicroBit::onListenerRegisteredEvent);
    messageBus.listen(DEVICE_ID_MESSAGE_BUS_LISTENER, ID_PIN_P0, this, &MicroBit::onP0ListenerRegisteredEvent, MESSAGE_BUS_LISTENER_IMMEDIATE);

#if CONFIG_ENABLED(DMESG_SERIAL_DEBUG) && DEVICE_DMESG_BUFFER_SIZE > 0
    codal_dmesg_set_flush_fn(microbit_dmesg_flush);
#endif

    status |= DEVICE_COMPONENT_STATUS_IDLE_TICK;
    status |= DEVICE_COMPONENT_STATUS_SYSTEM_TICK;

    // Set IRQ priorities for peripherals we use.
    // Note that low values have highest priority, and only 2, 3, 4, 5 and 7 are available with SoftDevice enabled.

    NVIC_SetPriority(TIMER1_IRQn, 7);         // System timer (general purpose)
    NVIC_SetPriority(TIMER2_IRQn, 5);         // ADC timer.
    NVIC_SetPriority(TIMER3_IRQn, 3);         // Cap touch.
    NVIC_SetPriority(TIMER4_IRQn, 3);         // Display and Light Sensing.

    NVIC_SetPriority(SAADC_IRQn, 5);          // Analogue to Digital Converter (microphone etc)
    NVIC_SetPriority(PWM0_IRQn, 5);           // General Purpose PWM on edge connector (servo, square wave sounds)
    NVIC_SetPriority(PWM1_IRQn, 4);           // PCM audio on speaker (high definition sound)
    NVIC_SetPriority(PWM2_IRQn, 3);           // Waveform Generation (neopixel)

    NVIC_SetPriority(RADIO_IRQn, 4);          // Packet radio
    NVIC_SetPriority(UARTE0_UART0_IRQn, 2);   // Serial port
    NVIC_SetPriority(GPIOTE_IRQn, 2);         // Pin interrupt events

    power.readInterfaceRequest();

#if CONFIG_ENABLED(DEVICE_BLE) && CONFIG_ENABLED(MICROBIT_BLE_PAIRING_MODE)
    int i=0;
    // Test if we need to enter BLE pairing mode
    // If a RebootMode Key has been set boot straight into BLE mode
    KeyValuePair* RebootMode = storage.get("RebootMode");
    KeyValuePair* flashIncomplete = storage.get("flashIncomplete");
    sleep(100);
    // Animation
    uint8_t x = 0; uint8_t y = 0;
    bool triple_reset = 0;

#if CONFIG_ENABLED(MICROBIT_TRIPLE_RESET_TO_PAIR)
    triple_reset = (microbit_no_init_memory_region.resetClickCount == 3);
#endif

    while (((triple_reset || (buttonA.isPressed() && buttonB.isPressed())) && i<25) || RebootMode != NULL || flashIncomplete != NULL)
    {
        display.image.setPixelValue(x,y,255);
        sleep(triple_reset ? 10 : 20);
        i++; x++;

        // Gradually fill screen
        if(x == 5){
          y++; x = 0;
        }

        if (i == 25 || RebootMode != NULL)
        {
            // Remove KV if it exists
            if(RebootMode != NULL){
                storage.remove("RebootMode");
            }
            delete RebootMode;
            delete flashIncomplete;
            microbit_no_init_memory_region.resetClickCount = 0;

            // Start the BLE stack, if it isn't already running.
            bleManager.init( ManagedString( microbit_friendly_name()), getSerial(), messageBus, storage, true);
            

#if CONFIG_ENABLED(MICROBIT_BLE_UTILITY_SERVICE_PAIRING)
            MICROBIT_DEBUG_DMESG( "UTILITY_SERVICE");
            MicroBitUtilityService::createShared( *ble, messageBus, storage, log);
#endif
            // Enter pairing mode, using the LED matrix for any necessary pairing operations
            bleManager.pairingMode(display, buttonA);
        }
    }
#endif

#if CONFIG_ENABLED(DEVICE_BLE) && CONFIG_ENABLED(MICROBIT_BLE_ENABLED)
    // Start the BLE stack, if it isn't already running.
    bleManager.init( ManagedString( microbit_friendly_name()), getSerial(), messageBus, storage, false);

    // Nested this a bit deeper for clarity... JV
    #if CONFIG_ENABLED(MICROBIT_BLE_UTILITY_SERVICE)
        MICROBIT_DEBUG_DMESG( "UTILITY_SERVICE");
        MicroBitUtilityService::createShared( *ble, messageBus, storage, log);
    #endif

    // Bring up the 64MHz external oscillator.
    sd_clock_hfclk_request();
#else
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
#endif

    // Deschedule for a little while, just to allow for any components that finialise initialisation
    // as a background task, and to allow the power mamanger to repsonse to background events from the KL27
    // before any user code begins running.
    
    sleep(10);

    return DEVICE_OK;
}

/**
  * A callback listener to disable default audio streaming to P0 if an event handler is registered on that pin.
  */
void MicroBit::onP0ListenerRegisteredEvent(Event evt)
{
    audio.setPinEnabled(false);
}

/**
  * A listener to perform actions as a result of Message Bus reflection.
  *
  * In some cases we want to perform lazy instantiation of components, such as
  * the compass and the accelerometer, where we only want to add them to the idle
  * fiber when someone has the intention of using these components.
  */
void MicroBit::onListenerRegisteredEvent(Event evt)
{
    switch(evt.value)
    {
        case DEVICE_ID_BUTTON_AB:
            // A user has registered to receive events from the buttonAB multibutton.
            // Disable click events from being generated by ButtonA and ButtonB, and defer the
            // control of this to the multibutton handler.
            //
            // This way, buttons look independent unless a buttonAB is requested, at which
            // point button A+B clicks can be correclty handled without breaking
            // causal ordering.
            buttonA.setEventConfiguration(DEVICE_BUTTON_SIMPLE_EVENTS);
            buttonB.setEventConfiguration(DEVICE_BUTTON_SIMPLE_EVENTS);
            buttonAB.setEventConfiguration(DEVICE_BUTTON_ALL_EVENTS);
            break;

        case DEVICE_ID_COMPASS:
            compass.requestUpdate();
            break;

        case DEVICE_ID_ACCELEROMETER:
        case DEVICE_ID_GESTURE:
            // A listener has been registered for the accelerometer.
            // The accelerometer uses lazy instantiation, we just need to read the data once to start it running.
            accelerometer.requestUpdate();
            break;

        case DEVICE_ID_THERMOMETER:
            // A listener has been registered for the thermometer.
            // The thermometer uses lazy instantiation, we just need to read the data once to start it running.
            //thermometer.updateSample();
            break;

        case DEVICE_ID_LIGHT_SENSOR:
            // A listener has been registered for the light sensor.
            // The light sensor uses lazy instantiation, we just need to read the data once to start it running.
            //lightSensor.updateSample();
            break;

        case DEVICE_ID_SYSTEM_LEVEL_DETECTOR:
            // A listener has been registered for the level detector.
            // The level detector uses lazy instantiation, we just need to read the data once to start it running.
            //audio.level->getValue();
            // The level detector requires that we enable constant listening, otherwise no events will be emitted.
            audio.levelSPL->activateForEvents( true );
            break;

        case DEVICE_ID_MICROPHONE:
            // A listener has been registered for the level detector SPL.
            // The level detector SPL uses lazy instantiation, we just need to read the data once to start it running.
            audio.levelSPL->getValue();
            break;
    }
}


/**
 * Perfom scheduler idle
*/
void MicroBit::schedulerIdle()
{
    if ( power.waitingForDeepSleep())
    {
#if CONFIG_ENABLED(DEVICE_BLE) && CONFIG_ENABLED(MICROBIT_BLE_ENABLED)
        if ( bleManager.getConnected())
        {
            power.cancelDeepSleep();
            target_wait_for_event();
            return;
        }
#endif
        if ( power.readyForDeepSleep())
        {
            if ( power.startDeepSleep() == DEVICE_OK)
                return;
        }
    }

    target_wait_for_event();
}

/**
  * A periodic callback invoked by the fiber scheduler idle thread.
  * We use this for any low priority, backgrounf housekeeping.
  *
  */
void MicroBit::idleCallback()
{
#if CONFIG_ENABLED(DMESG_SERIAL_DEBUG)
#if DEVICE_DMESG_BUFFER_SIZE > 0
    codal_dmesg_flush();
#endif
#endif
}

/**
  * Periodic callback from Device system timer.
  *
  */
void MicroBit::periodicCallback()
{
    // Zero our reset_count once the micro:bit has been running for half a second
    static int timeout = 500 / (SCHEDULER_TICK_PERIOD_US/1000);

    if (timeout-- == 0 && microbit_no_init_memory_region.resetClickCount != 0)
    {
        microbit_no_init_memory_region.resetClickCount = 0;
        status &= ~DEVICE_COMPONENT_STATUS_SYSTEM_TICK;
    }
}

/**
 * Determines if any persistent storage needs to be erased following the reprogramming
 * of the micro:bit.
 *
 * @param forceErase Force an erase of user data, even if we have not detected a reflash event.
 */
void MicroBit::eraseUserStorage(bool forceErase)
{
    uint32_t zero = 0;
    uint32_t reset_value;
    NRF52FlashManager f(0, 128, 4096);

    f.read(&reset_value, (uint32_t) &reflash_status, 1);

    // If there is no indication of a reflash event, there's nothing to do.
    if (!(reset_value || forceErase))
        return;

    // Clear our flag if we have been reflashed
    if (reset_value)
        f.write((uint32_t) &reflash_status, &zero, 1);

    // Determine if our flash contains a recognised file system. If so, invalidate it.
#if CONFIG_ENABLED(CONFIG_MICROBIT_ERASE_USER_DATA_ON_REFLASH)
    log.invalidate();
#endif
}

void microbit_dmesg_flush()
{
#if CONFIG_ENABLED(DMESG_SERIAL_DEBUG)
#if DEVICE_DMESG_BUFFER_SIZE > 0
    if (codalLogStore.ptr > 0 && microbit_device_instance)
    {
        for (uint32_t i=0; i<codalLogStore.ptr; i++)
            ((MicroBit *)microbit_device_instance)->serial.putc(codalLogStore.buffer[i]);

        codalLogStore.ptr = 0;
    }
#endif
#endif
}

