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

/**
  * Class definition for MicroBit NEXT IO.
  * Represents a collection of all I/O pins on the device.
  */

#include "CodalConfig.h"
#include "CodalDmesg.h"
#include "MicroBitIO.h"

using namespace codal;

/**
  * Constructor.
  *
  * Create a representation of all given I/O pins on the edge connector
  *
  * Accepts a sequence of unique ID's used to distinguish events raised
  * by MicroBitPin instances on the default EventModel.
  */
MicroBitIO::MicroBitIO(NRF52ADC &a, TouchSensor &s) :

    // Edge Connector Pins
    P0(ID_PIN_P0, P0_02, PIN_CAPABILITY_AD),
    P1(ID_PIN_P1, P0_03, PIN_CAPABILITY_AD),
    P2(ID_PIN_P2, P0_04, PIN_CAPABILITY_AD),
    P3(ID_PIN_P3, P0_31, PIN_CAPABILITY_AD),
    P4(ID_PIN_P4, P0_28, PIN_CAPABILITY_AD),
    P5(ID_PIN_P5, P0_14, PIN_CAPABILITY_AD),
    P6(ID_PIN_P6, P1_5, PIN_CAPABILITY_AD),
    P7(ID_PIN_P7, P0_11, PIN_CAPABILITY_AD),
    P8(ID_PIN_P8, P0_10, PIN_CAPABILITY_AD),
    P9(ID_PIN_P9, P0_09, PIN_CAPABILITY_AD),
    P10(ID_PIN_P10, P0_30, PIN_CAPABILITY_AD),
    P11(ID_PIN_P11, P0_23, PIN_CAPABILITY_AD),
    P12(ID_PIN_P12, P0_12, PIN_CAPABILITY_AD),
    P13(ID_PIN_P13, P0_17, PIN_CAPABILITY_AD),
    P14(ID_PIN_P14, P0_01, PIN_CAPABILITY_AD),
    P15(ID_PIN_P15, P0_13, PIN_CAPABILITY_AD),
    P16(ID_PIN_P16, P1_02, PIN_CAPABILITY_AD),
    P19(ID_PIN_P19, P0_26, PIN_CAPABILITY_AD),
    P20(ID_PIN_P20, P1_00, PIN_CAPABILITY_AD),

    // Other exposed pins
    logo(ID_PIN_LOGO, P1_04, PIN_CAPABILITY_AD),

    // Internal Pins
    speaker(ID_PIN_SPEAKER, P0_00, PIN_CAPABILITY_AD),
    runmic(ID_PIN_RUNMIC, P0_20, PIN_CAPABILITY_AD),
    microphone(ID_PIN_MIC, P0_05, PIN_CAPABILITY_AD),
    sda(ID_PIN_SDA, P0_16, PIN_CAPABILITY_AD),
    scl(ID_PIN_SCL, P0_08, PIN_CAPABILITY_AD),
    row1(ID_PIN_ROW1, P0_21, PIN_CAPABILITY_AD),
    row2(ID_PIN_ROW2, P0_22, PIN_CAPABILITY_AD),
    row3(ID_PIN_ROW3, P0_15, PIN_CAPABILITY_AD),
    row4(ID_PIN_ROW4, P0_24, PIN_CAPABILITY_AD),
    row5(ID_PIN_ROW5, P0_19, PIN_CAPABILITY_AD),
    usbTx(ID_PIN_USBTX, MICROBIT_PIN_UART_TX, PIN_CAPABILITY_DIGITAL),
    usbRx(ID_PIN_USBRX, MICROBIT_PIN_UART_RX, PIN_CAPABILITY_DIGITAL),
    irq1(ID_PIN_IRQ1, P0_25, PIN_CAPABILITY_AD),

    // Aliases
    col1(P4),
    col2(P7),
    col3(P3),
    col4(P6),
    col5(P10),
    buttonA(P5),
    buttonB(P11),
    face(logo)
{
    pins = 33;
    NRF52Pin::adc = &a;
    NRF52Pin::touchSensor = &s;

    // Ensure all internal pins are configured with no pull resistors.
    for (int i=19; i<pins; i++)
        pin[i].setPull(PullMode::None);

    // Ensure all internal multiplexed pins are configured with no pull resistors.
    col1.setPull(PullMode::None);
    col2.setPull(PullMode::None);
    col3.setPull(PullMode::None);
    col4.setPull(PullMode::None);
    col5.setPull(PullMode::None);
    buttonA.setPull(PullMode::None);
    buttonB.setPull(PullMode::None);

    savedStatus = ManagedBuffer(pins);
}

#ifdef NRF_P1
#define PORT (name < 32 ? NRF_P0 : NRF_P1)
#define PIN ((name) & 31)
#define NUM_PINS 48
#else
#define PORT (NRF_P0)
#define PIN (name)
#define NUM_PINS 32
#endif

/**
 * Puts the component in (or out of) sleep (low power) mode.
 */
int MicroBitIO::setSleep(bool doSleep)
{
    int name;

    if (doSleep)
    {
        // Record current state of pins, so we can return the configuration to the same state later.       
        for (int i = 0; i < pins; i++)
        {
            name = pin[i].name;
            savedStatus[i] = 0;

            // Disable any interrupts associated with this GPIO pin
            if ((PORT->PIN_CNF[PIN] & GPIO_PIN_CNF_SENSE_Msk) == (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos))
                savedStatus[i] |= IO_SAVED_STATUS_DETECT_LOW_ENABLED;

            // Disable any interrupts associated with this GPIO pin
            if ((PORT->PIN_CNF[PIN] & GPIO_PIN_CNF_SENSE_Msk) == (GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos))
                savedStatus[i] |= IO_SAVED_STATUS_DETECT_HIGH_ENABLED;

            pin[i].setDetect(GPIO_PIN_CNF_SENSE_Disabled);

#if CONFIG_ENABLED(DEEP_SLEEP_GPIO_OUTPUTS)
            // If this pin is in GPIO output mode, record its value and set to high impedance input.
            // Other components are reponsible for storing/restoring the state of higher level peripherals.
            if (pin[i].isOutput())
            {
                savedStatus[i] |= PORT->OUT & (1 << PIN) ? IO_SAVED_STATUS_OUTPUT_HI : IO_SAVED_STATUS_OUTPUT_LO;
                PORT->PIN_CNF[PIN] = 0x00000002;
            }
#endif

        }
    }

    // Restore any saved GPIO state
    if (!doSleep)
    {
        for (int i = 0; i < pins; i++)
        {
            name = pin[i].name;

#if CONFIG_ENABLED(DEEP_SLEEP_GPIO_OUTPUTS)
            // Re-enable GPIO outputs that were placed into high impedance mode.
            if (savedStatus[i] & IO_SAVED_STATUS_OUTPUT_HI){
                PORT->OUTSET = 1 << PIN;
                PORT->PIN_CNF[PIN] = PORT->PIN_CNF[PIN] | 1;
            }

            if (savedStatus[i] & IO_SAVED_STATUS_OUTPUT_LO){
                PORT->OUTCLR = 1 << PIN;
                PORT->PIN_CNF[PIN] = PORT->PIN_CNF[PIN] | 1;
            }
#endif

            // Re-enable any interrupts associated with this GPIO pin
            if(savedStatus[i] & IO_SAVED_STATUS_DETECT_LOW_ENABLED)
                pin[i].setDetect(GPIO_PIN_CNF_SENSE_Low);

            if(savedStatus[i] & IO_SAVED_STATUS_DETECT_HIGH_ENABLED)
                pin[i].setDetect(GPIO_PIN_CNF_SENSE_High);
        }
    }
   
    return DEVICE_OK;
}
