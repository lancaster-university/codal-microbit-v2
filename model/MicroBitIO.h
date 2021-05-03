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

#ifndef MICROBIT_IO_H
#define MICROBIT_IO_H

#include "CodalConfig.h"
#include "NRF52Pin.h"
#include "NRF52ADC.h"
#include "TouchSensor.h"

//
// Unique Pin number for each pin (synonymous with mbedos PinName)
//
typedef enum {
    p0  = 0,
    p1  = 1,
    p2  = 2,
    p3  = 3,
    p4  = 4,
    p5  = 5,
    p6  = 6,
    p7  = 7,
    p8  = 8,
    p9  = 9,
    p10 = 10,
    p11 = 11,
    p12 = 12,
    p13 = 13,
    p14 = 14,
    p15 = 15,
    p16 = 16,
    p17 = 17,
    p18 = 18,
    p19 = 19,
    p20 = 20,
    p21 = 21,
    p22 = 22,
    p23 = 23,
    p24 = 24,
    p25 = 25,
    p26 = 26,
    p27 = 27,
    p28 = 28,
    p29 = 29,
    p30 = 30,
    p31 = 31,
    p32 = 32,
    p33 = 33,
    p34 = 34,
    p35 = 35,
    p36 = 36,
    p37 = 37,
    p38 = 38,
    p39 = 39,
    p40 = 40,
    p41 = 41,
    p42 = 42,
    p43 = 43,
    p44 = 44,
    p45 = 45,
    p46 = 46,
    p47 = 47,

    P0_0  = p0,
    P0_1  = p1,
    P0_2  = p2,
    P0_3  = p3,
    P0_4  = p4,
    P0_5  = p5,
    P0_6  = p6,
    P0_7  = p7,

    P0_8  = p8,
    P0_9  = p9,
    P0_10 = p10,
    P0_11 = p11,
    P0_12 = p12,
    P0_13 = p13,
    P0_14 = p14,
    P0_15 = p15,

    P0_16 = p16,
    P0_17 = p17,
    P0_18 = p18,
    P0_19 = p19,
    P0_20 = p20,
    P0_21 = p21,
    P0_22 = p22,
    P0_23 = p23,

    P0_24 = p24,
    P0_25 = p25,
    P0_26 = p26,
    P0_27 = p27,
    P0_28 = p28,
    P0_29 = p29,
    P0_30 = p30,
    P0_31 = p31,

    P1_0 = p32,
    P1_1 = p33,
    P1_2 = p34,
    P1_3 = p35,
    P1_4 = p36,
    P1_5 = p37,
    P1_6 = p38,
    P1_7 = p39,

    P1_8 = p40,
    P1_9 = p41,
    P1_10 = p42,
    P1_11 = p43,
    P1_12 = p44,
    P1_13 = p45,
    P1_14 = p46,
    P1_15 = p47,

    // IDENTITY MAPPINGS
    P0_00 = P0_0,
    P0_01 = P0_1,
    P0_02 = P0_2,
    P0_03 = P0_3,
    P0_04 = P0_4,
    P0_05 = P0_5,
    P0_06 = P0_6,
    P0_07 = P0_7,
    P0_08 = P0_8,
    P0_09 = P0_9,
    P1_00 = P1_0,
    P1_01 = P1_1,
    P1_02 = P1_2,
    P1_03 = P1_3,
    P1_04 = P1_4,
    P1_05 = P1_5,
    P1_06 = P1_6,
    P1_07 = P1_7,
    P1_08 = P1_8,
    P1_09 = P1_9,

    // Buttons A and B
    BUTTONA = P0_14,
    BUTTONB = P0_23,

    // LED matrix
    ROW_1 = P0_21,
    ROW_2 = P0_22,
    ROW_3 = P0_15,
    ROW_4 = P0_24,
    ROW_5 = P0_19,
    COL_1 = P0_28,
    COL_2 = P0_11,
    COL_3 = P0_31,
    COL_4 = P1_5,
    COL_5 = P0_30,

    USB_UART_RX = p8,
    USB_UART_TX = p6,

    SPI_PSELMOSI0 = P1_13,
    SPI_PSELMISO0 = P1_14,
    SPI_PSELSS0   = P1_12,
    SPI_PSELSCK0  = P1_15,

    SPI_PSELMOSI1 = P1_2,
    SPI_PSELMISO1 = P1_3,
    SPI_PSELSS1   = P1_1,
    SPI_PSELSCK1  = P1_4,

    SPIS_PSELMOSI = P1_2,
    SPIS_PSELMISO = P1_3,
    SPIS_PSELSS   = P1_1,
    SPIS_PSELSCK  = P1_4,

    I2C_SDA0 = p26,
    I2C_SCL0 = p27,


    /**** QSPI pins ****/
    QSPI1_IO0 = P0_20,
    QSPI1_IO1 = P0_21,
    QSPI1_IO2 = P0_22,
    QSPI1_IO3 = P0_23,
    QSPI1_SCK = P0_19,
    QSPI1_CSN = P0_17,

    /**** QSPI FLASH pins ****/
    QSPI_FLASH1_IO0 = QSPI1_IO0,
    QSPI_FLASH1_IO1 = QSPI1_IO1,
    QSPI_FLASH1_IO2 = QSPI1_IO2,
    QSPI_FLASH1_IO3 = QSPI1_IO3,
    QSPI_FLASH1_SCK = QSPI1_SCK,
    QSPI_FLASH1_CSN = QSPI1_CSN,

    // Not connected
    NC = (int)0xFFFFFFFF
} PinName;



//Edge Connector
#define MICROBIT_PIN_P0                             P0_02
#define MICROBIT_PIN_P1                             P0_03
#define MICROBIT_PIN_P2                             P0_04
#define MICROBIT_PIN_P3                             P0_31
#define MICROBIT_PIN_P4                             P0_28
#define MICROBIT_PIN_P5                             P0_14
#define MICROBIT_PIN_P6                             P1_05
#define MICROBIT_PIN_P7                             P0_11
#define MICROBIT_PIN_P8                             P0_10
#define MICROBIT_PIN_P9                             P0_09
#define MICROBIT_PIN_P10                            P0_30
#define MICROBIT_PIN_P11                            P0_23
#define MICROBIT_PIN_P12                            P0_12
#define MICROBIT_PIN_P13                            P0_17
#define MICROBIT_PIN_P14                            P0_01
#define MICROBIT_PIN_P15                            P0_13
#define MICROBIT_PIN_P16                            P1_02
#define MICROBIT_PIN_P19                            P0_26
#define MICROBIT_PIN_P20                            P1_00

// User Input
#define MICROBIT_PIN_BUTTON_A                       P0_14
#define MICROBIT_PIN_BUTTON_B                       P0_23
#define MICROBIT_PIN_LOGO_TOUCH                     P1_04

// FOR MakeCode extension compat. Delete ASAP.
#define MICROBIT_PIN_FACE_TOUCH                     MICROBIT_PIN_LOGO_TOUCH

// Audio
#define MICROBIT_PIN_RUN_MIC                        P0_20
#define MICROBIT_PIN_MIC_IN                         P0_05
#define MICROBIT_PIN_SPEAKER                        P0_00

// Display
#define MICROBIT_PIN_ROW1                           P0_21
#define MICROBIT_PIN_ROW2                           P0_22
#define MICROBIT_PIN_ROW3                           P0_15
#define MICROBIT_PIN_ROW4                           P0_24
#define MICROBIT_PIN_ROW5                           P0_19
#define MICROBIT_PIN_COL1                           P0_28
#define MICROBIT_PIN_COL2                           P0_11
#define MICROBIT_PIN_COL3                           P0_31
#define MICROBIT_PIN_COL4                           P1_05
#define MICROBIT_PIN_COL5                           P0_30

// I2C (internal)
#define MICROBIT_PIN_INT_SDA                        P0_16
#define MICROBIT_PIN_INT_SCL                        P0_08
#define MICROBIT_PIN_SENSOR_DATA_READY              P0_25
// I2C (external)
#define MICROBIT_PIN_EXT_SDA                        P1_00
#define MICROBIT_PIN_EXT_SCL                        P0_26

// SPI
#define MICROBIT_PIN_MOSI                           P0_13
#define MICROBIT_PIN_MISO                           P0_01
#define MICROBIT_PIN_SCK                            P0_17

// UART
#define MICROBIT_PIN_UART_TX                        P0_06
#define MICROBIT_PIN_UART_RX                        P1_08

// Unused
#define MICROBIT_PIN_BUTTON_RESET                   -1

//
// Component IDs for each pin.
// The can be user defined, but uniquely identify a pin when using the eventing APIs/
//
#define ID_PIN_P0        (DEVICE_ID_IO_P0 + 0)
#define ID_PIN_P1        (DEVICE_ID_IO_P0 + 1)
#define ID_PIN_P2        (DEVICE_ID_IO_P0 + 2)
#define ID_PIN_P3        (DEVICE_ID_IO_P0 + 3)
#define ID_PIN_P4        (DEVICE_ID_IO_P0 + 4)
#define ID_PIN_P5        (DEVICE_ID_IO_P0 + 5)
#define ID_PIN_P6        (DEVICE_ID_IO_P0 + 6)
#define ID_PIN_P7        (DEVICE_ID_IO_P0 + 7)
#define ID_PIN_P8        (DEVICE_ID_IO_P0 + 8)
#define ID_PIN_P9        (DEVICE_ID_IO_P0 + 9)
#define ID_PIN_P10       (DEVICE_ID_IO_P0 + 10)
#define ID_PIN_P11       (DEVICE_ID_IO_P0 + 11)
#define ID_PIN_P12       (DEVICE_ID_IO_P0 + 12)
#define ID_PIN_P13       (DEVICE_ID_IO_P0 + 13)
#define ID_PIN_P14       (DEVICE_ID_IO_P0 + 14)
#define ID_PIN_P15       (DEVICE_ID_IO_P0 + 15)
#define ID_PIN_P16       (DEVICE_ID_IO_P0 + 16)
#define ID_PIN_P17       (DEVICE_ID_IO_P0 + 17)
#define ID_PIN_P18       (DEVICE_ID_IO_P0 + 18)
#define ID_PIN_P19       (DEVICE_ID_IO_P0 + 19)
#define ID_PIN_P20       (DEVICE_ID_IO_P0 + 20)
#define ID_PIN_LOGO      (DEVICE_ID_IO_P0 + 21)
#define ID_PIN_SPEAKER   (DEVICE_ID_IO_P0 + 22)
#define ID_PIN_RUNMIC    (DEVICE_ID_IO_P0 + 23)
#define ID_PIN_SDA       (DEVICE_ID_IO_P0 + 24)
#define ID_PIN_SCL       (DEVICE_ID_IO_P0 + 25)
#define ID_PIN_ROW1      (DEVICE_ID_IO_P0 + 26)
#define ID_PIN_ROW2      (DEVICE_ID_IO_P0 + 27)
#define ID_PIN_ROW3      (DEVICE_ID_IO_P0 + 28)
#define ID_PIN_ROW4      (DEVICE_ID_IO_P0 + 29)
#define ID_PIN_ROW5      (DEVICE_ID_IO_P0 + 30)
#define ID_PIN_USBTX     (DEVICE_ID_IO_P0 + 31)
#define ID_PIN_USBRX     (DEVICE_ID_IO_P0 + 32)
#define ID_PIN_IRQ1      (DEVICE_ID_IO_P0 + 33)
#define ID_PIN_MIC       (DEVICE_ID_IO_P0 + 34)
#define ID_PIN_P35       (DEVICE_ID_IO_P0 + 35)
#define ID_PIN_P36       (DEVICE_ID_IO_P0 + 36)
#define ID_PIN_P37       (DEVICE_ID_IO_P0 + 37)
#define ID_PIN_P38       (DEVICE_ID_IO_P0 + 38)
#define ID_PIN_P39       (DEVICE_ID_IO_P0 + 39)
#define ID_PIN_P40       (DEVICE_ID_IO_P0 + 40)
#define ID_PIN_P41       (DEVICE_ID_IO_P0 + 41)
#define ID_PIN_P42       (DEVICE_ID_IO_P0 + 42)
#define ID_PIN_P43       (DEVICE_ID_IO_P0 + 43)
#define ID_PIN_P44       (DEVICE_ID_IO_P0 + 44)
#define ID_PIN_P45       (DEVICE_ID_IO_P0 + 45)
#define ID_PIN_P46       (DEVICE_ID_IO_P0 + 46)
#define ID_PIN_P47       (DEVICE_ID_IO_P0 + 47)

// For MakeCode extension compat. Delete ASAP.
#define ID_PIN_FACE      (ID_PIN_LOGO)

//
// Saved status values to restore GPIO configurations after deep sleep
//
#define IO_SAVED_STATUS_OUTPUT_NA               0
#define IO_SAVED_STATUS_OUTPUT_LO               1
#define IO_SAVED_STATUS_OUTPUT_HI               2
#define IO_SAVED_STATUS_DETECT_LOW_ENABLED      4
#define IO_SAVED_STATUS_DETECT_HIGH_ENABLED     8

namespace codal
{
    /**
     * Represents a collection of all I/O pins exposed by the device.
     */
    class MicroBitIO : public CodalComponent
    {
        private:
            ManagedBuffer     savedStatus;

        public:
            // Number of pins in use.
            int               pins;

            // Enumeration of all pins, ordered by edge connector.
            NRF52Pin          pin[0];

            // Edge Connector Pins
            NRF52Pin          P0;           // P0_02
            NRF52Pin          P1;           // P0_03
            NRF52Pin          P2;           // P0_04
            NRF52Pin          P3;           // P0_31
            NRF52Pin          P4;           // P0_28
            NRF52Pin          P5;           // P0_14
            NRF52Pin          P6;           // P1_05
            NRF52Pin          P7;           // P0_11
            NRF52Pin          P8;           // P0_10
            NRF52Pin          P9;           // P0_09
            NRF52Pin          P10;          // P0_30
            NRF52Pin          P11;          // P0_23
            NRF52Pin          P12;          // P0_12
            NRF52Pin          P13;          // P0_17
            NRF52Pin          P14;          // P0_01
            NRF52Pin          P15;          // P0_13
            NRF52Pin          P16;          // P1_02
            NRF52Pin          P19;          // P0_26
            NRF52Pin          P20;          // P1_00

            // Other exposed pins
            NRF52Pin          logo;         // P1_04

            // Internal Pins
            NRF52Pin          speaker;      // P0_00
            NRF52Pin          runmic;       // P0_20
            NRF52Pin          microphone;   // P0_05
            NRF52Pin          sda;          // P0_16
            NRF52Pin          scl;          // P0_08
            NRF52Pin          row1;         // P0_21
            NRF52Pin          row2;         // P0_22
            NRF52Pin          row3;         // P0_15
            NRF52Pin          row4;         // P0_24
            NRF52Pin          row5;         // P0_19
            NRF52Pin          usbTx;
            NRF52Pin          usbRx;
            NRF52Pin          irq1;         // P0_25

            // Aliases
            NRF52Pin&         col1;
            NRF52Pin&         col2;
            NRF52Pin&         col3;
            NRF52Pin&         col4;
            NRF52Pin&         col5;
            NRF52Pin&         buttonA;
            NRF52Pin&         buttonB;
            NRF52Pin&         face;         // Temporary compat for MakeCode extension. Delete ASAP.


            /**
             * Constructor.
             *
             * Create a representation of all given I/O pins on the edge connector
             *
             * Accepts a sequence of unique ID's used to distinguish events raised
             * by MicroBitPin instances on the default EventModel.
             */
            MicroBitIO(NRF52ADC &a, TouchSensor &s);

            /**
             * Puts the component in (or out of) sleep (low power) mode.
             */
            virtual int setSleep(bool doSleep) override;

            /**
             * Perform functions related to deep sleep wake-up.
             */
            virtual int manageWakeUp( wakeUpReason reason, wakeUpResult *result) override;
    };
}

#endif
