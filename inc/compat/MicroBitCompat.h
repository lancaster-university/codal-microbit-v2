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

#ifndef MICROBIT_COMPAT_H
#define MICROBIT_COMPAT_H

#include "CodalConfig.h"
#include "ErrorNo.h"
#include "codal-core/inc/types/Event.h"
#include "codal-core/inc/core/CodalComponent.h"
#include "codal-core/inc/core/CodalDmesg.h"
#include "codal-core/inc/driver-models/Accelerometer.h"
#include "codal-core/inc/drivers/Button.h"
#include "codal-core/inc/driver-models/Timer.h"
#include "codal-core/inc/drivers/MultiButton.h"
#include "codal-core/inc/driver-models/Compass.h"
#include "codal-core/inc/driver-models/Serial.h"
#include "codal-core/inc/drivers/LEDMatrix.h"
#include "codal-core/inc/drivers/AnimatedDisplay.h"
#include "codal-core/inc/driver-models/I2C.h"
#include "MicroBitIO.h"
#include "NRF52Pin.h"
#include "NRF52I2C.h"
#include "NRF52SPI.h"

//
// Historic mbed types
//
typedef enum {
    PullNone = 0,
    PullDown = 1,
    PullUp = 3,
    PullDefault = PullUp
} PinMode;


//
// MicroBit types
//
typedef codal::NRF52Pin MicroBitPin;
typedef codal::NRF52I2C MicroBitI2C;
typedef codal::Event MicroBitEvent;
typedef codal::Listener MicroBitListener;
typedef codal::Image MicroBitImage;
typedef codal::Button MicroBitButton;
typedef codal::SerialMode MicroBitSerialMode;
typedef codal::CodalComponent MicroBitComponent;
typedef codal::EventLaunchMode MicroBitEventLaunchMode;

//
// MICROBIT specific error codes
//
#define MICROBIT_BUSY                           DEVICE_BUSY
#define MICROBIT_CANCELLED                      DEVICE_CANCELLED
#define MICROBIT_CALIBRATION_IN_PROGRESS        DEVICE_CALIBRATION_IN_PROGRESS
#define MICROBIT_CALIBRATION_REQUIRED           DEVICE_CALIBRATION_REQUIRED
#define MICROBIT_HEAP_ERROR                     DEVICE_HEAP_ERROR
#define MICROBIT_I2C_ERROR                      DEVICE_I2C_ERROR
#define MICROBIT_INVALID_PARAMETER              DEVICE_INVALID_PARAMETER
#define MICROBIT_NO_DATA                        DEVICE_NO_DATA
#define MICROBIT_NO_RESOURCES                   DEVICE_NO_RESOURCES
#define MICROBIT_NOT_SUPPORTED                  DEVICE_NOT_SUPPORTED
#define MICROBIT_NULL_DEREFERENCE               DEVICE_NULL_DEREFERENCE
#define MICROBIT_OK                             DEVICE_OK
#define MICROBIT_OOM                            DEVICE_OOM
#define MICROBIT_SERIAL_IN_USE                  DEVICE_SERIAL_IN_USE


//
// MICROBIT CONFIG options
//

// BLE related constants - not yet supported.
//#define MICROBIT_DAL_BLUETOOTH_ADVERTISING_TIMEOUT
//#define MICROBIT_DAL_BLUETOOTH_DEVICE_INFO_SERVICE
//#define MICROBIT_DAL_BLUETOOTH_DFU_SERVICE
//#define MICROBIT_DAL_BLUETOOTH_EDDYSTONE_UID
//#define MICROBIT_DAL_BLUETOOTH_EDDYSTONE_URL
//#define MICROBIT_DAL_BLUETOOTH_ENABLED
//#define MICROBIT_DAL_BLUETOOTH_EVENT_SERVICE
//#define MICROBIT_DAL_BLUETOOTH_OPEN
//#define MICROBIT_DAL_BLUETOOTH_PAIRING_MODE
//#define MICROBIT_DAL_BLUETOOTH_PRIVATE_ADDRESSING
//#define MICROBIT_DAL_BLUETOOTH_SECURITY_LEVEL
//#define MICROBIT_DAL_BLUETOOTH_TX_POWER
//#define MICROBIT_DAL_BLUETOOTH_WHITELIST



//
// MICROBIT specific constants with CODAL mappings where appropriate
//

#define MICROBIT_ACCELEROMETER_3G_THRESHOLD                     ACCELEROMETER_3G_THRESHOLD
#define MICROBIT_ACCELEROMETER_3G_TOLERANCE                     ACCELEROMETER_3G_TOLERANCE
#define MICROBIT_ACCELEROMETER_6G_THRESHOLD                     ACCELEROMETER_6G_THRESHOLD
#define MICROBIT_ACCELEROMETER_6G_TOLERANCE                     ACCELEROMETER_6G_TOLERANCE
#define MICROBIT_ACCELEROMETER_8G_THRESHOLD                     ACCELEROMETER_8G_THRESHOLD
#define MICROBIT_ACCELEROMETER_8G_TOLERANCE                     ACCELEROMETER_8G_TOLERANCE
#define MICROBIT_ACCELEROMETER_FREEFALL_THRESHOLD               ACCELEROMETER_FREEFALL_THRESHOLD
#define MICROBIT_ACCELEROMETER_FREEFALL_TOLERANCE               ACCELEROMETER_FREEFALL_TOLERANCE
#define MICROBIT_ACCELEROMETER_GESTURE_DAMPING                  ACCELEROMETER_GESTURE_DAMPING
#define MICROBIT_ACCELEROMETER_REST_THRESHOLD                   ACCELEROMETER_REST_THRESHOLD
#define MICROBIT_ACCELEROMETER_REST_TOLERANCE                   ACCELEROMETER_REST_TOLERANCE
#define MICROBIT_ACCELEROMETER_SHAKE_COUNT_THRESHOLD            ACCELEROMETER_SHAKE_COUNT_THRESHOLD
#define MICROBIT_ACCELEROMETER_SHAKE_DAMPING                    ACCELEROMETER_SHAKE_DAMPING
#define MICROBIT_ACCELEROMETER_SHAKE_RTX                        ACCELEROMETER_SHAKE_RTX
#define MICROBIT_ACCELEROMETER_SHAKE_TOLERANCE                  ACCELEROMETER_SHAKE_TOLERANCE
#define MICROBIT_ACCELEROMETER_TILT_TOLERANCE                   ACCELEROMETER_TILT_TOLERANCE
#define MICROBIT_COMPONENT_RUNNING                              DEVICE_COMPONENT_RUNNING
#define MICROBIT_DBG                                            CODAL_DEBUG
#define MICROBIT_DEFAULT_PRINT_SPEED                            DISPLAY_DEFAULT_PRINT_SPEED
#define MICROBIT_DEFAULT_PULLMODE                               DEVICE_DEFAULT_PULLMODE
#define MICROBIT_DEFAULT_PWM_PERIOD                             DEVICE_DEFAULT_PWM_PERIOD
#define MICROBIT_DEFAULT_SCROLL_SPEED                           DISPLAY_DEFAULT_SCROLL_SPEED
#define MICROBIT_DEFAULT_SCROLL_STRIDE                          DISPLAY_DEFAULT_SCROLL_STRIDE
#define MICROBIT_DEFAULT_SERIAL_MODE                            DEVICE_DEFAULT_SERIAL_MODE
#define MICROBIT_DISPLAY_DEFAULT_AUTOCLEAR                      DISPLAY_DEFAULT_AUTOCLEAR
#define MICROBIT_DISPLAY_DEFAULT_BRIGHTNESS                     LED_MATRIX_DEFAULT_BRIGHTNESS
#define MICROBIT_DISPLAY_GREYSCALE_BIT_DEPTH                    LED_MATRIX_GREYSCALE_BIT_DEPTH
#define MICROBIT_DISPLAY_MAXIMUM_BRIGHTNESS                     LED_MATRIX_MAXIMUM_BRIGHTNESS
#define MICROBIT_DISPLAY_MINIMUM_BRIGHTNESS                     LED_MATRIX_MINIMUM_BRIGHTNESS
#define MICROBIT_DISPLAY_ROTATION_0                             MATRIX_DISPLAY_ROTATION_0
#define MICROBIT_DISPLAY_ROTATION_180                           MATRIX_DISPLAY_ROTATION_180
#define MICROBIT_DISPLAY_ROTATION_270                           MATRIX_DISPLAY_ROTATION_270
#define MICROBIT_DISPLAY_ROTATION_90                            MATRIX_DISPLAY_ROTATION_90
#define MICROBIT_EVENT_DEFAULT_LAUNCH_MODE                      DEVICE_EVENT_DEFAULT_LAUNCH_MODE
#define MICROBIT_DISPLAY_ANIMATE_DEFAULT_POS                    DISPLAY_ANIMATE_DEFAULT_POS
#define MICROBIT_FONT_ASCII_END                                 BITMAP_FONT_ASCII_END
#define MICROBIT_FONT_ASCII_START                               BITMAP_FONT_ASCII_START
#define MICROBIT_FONT_HEIGHT                                    BITMAP_FONT_HEIGHT
#define MICROBIT_FONT_WIDTH                                     BITMAP_FONT_WIDTH
#define MICROBIT_HEAP_ALLOCATOR                                 DEVICE_HEAP_ALLOCATOR
#define MICROBIT_HEAP_BLOCK_SIZE                                DEVICE_HEAP_BLOCK_SIZE
#define MICROBIT_HEAP_DBG                                       (CODAL_DEBUG == CODAL_DEBUG_HEAP)
#define MICROBIT_HEAP_END                                       CODAL_HEAP_END
#define MICROBIT_I2C_MAX_RETRIES                                1
#define MICROBIT_IDLE_COMPONENTS                                DEVICE_COMPONENT_COUNT
#define MICROBIT_SYSTEM_COMPONENTS                              DEVICE_COMPONENT_COUNT
#define MICROBIT_PIN_DEFAULT_SERVO_CENTER                       DEVICE_PIN_DEFAULT_SERVO_CENTER
#define MICROBIT_PIN_DEFAULT_SERVO_RANGE                        DEVICE_PIN_DEFAULT_SERVO_RANGE
#define MICROBIT_PIN_EVENT_NONE                                 DEVICE_PIN_EVENT_NONE
#define MICROBIT_PIN_EVENT_ON_EDGE                              DEVICE_PIN_EVENT_ON_EDGE
#define MICROBIT_PIN_EVENT_ON_PULSE                             DEVICE_PIN_EVENT_ON_PULSE
#define MICROBIT_PIN_EVENT_ON_TOUCH                             DEVICE_PIN_EVENT_ON_TOUCH
#define MICROBIT_PIN_EVT_FALL                                   DEVICE_PIN_EVT_FALL
#define MICROBIT_PIN_EVT_PULSE_HI                               DEVICE_PIN_EVT_PULSE_HI
#define MICROBIT_PIN_EVT_PULSE_LO                               DEVICE_PIN_EVT_PULSE_LO
#define MICROBIT_PIN_EVT_RISE                                   DEVICE_PIN_EVT_RISE
#define MICROBIT_PIN_MAX_OUTPUT                                 DEVICE_PIN_MAX_OUTPUT
#define MICROBIT_PIN_MAX_SERVO_RANGE                            DEVICE_PIN_MAX_SERVO_RANGE

//
// BLE related constants - not yet supported.
//

//#define MICROBIT_BLE_ADVERTISING_TIMEOUT
//#define MICROBIT_BLE_DEFAULT_TX_POWER
//#define MICROBIT_BLE_DEVICE_INFORMATION_SERVICE
//#define MICROBIT_BLE_DFU_SERVICE
//#define MICROBIT_BLE_DISCONNECT_AFTER_PAIRING_DELAY
//#define MICROBIT_BLE_EDDYSTONE_ADV_INTERVAL
//#define MICROBIT_BLE_EDDYSTONE_DEFAULT_POWER
//#define MICROBIT_BLE_EDDYSTONE_UID
//#define MICROBIT_BLE_EDDYSTONE_URL
//#define MICROBIT_BLE_ENABLE_BONDING
//#define MICROBIT_BLE_ENABLED
//#define MICROBIT_BLE_EVENT_SERVICE
//#define MICROBIT_BLE_EVT_CONNECTED
//#define MICROBIT_BLE_EVT_DISCONNECTED
//#define MICROBIT_BLE_FIRMWARE_VERSION
//#define MICROBIT_BLE_HARDWARE_VERSION
//#define MICROBIT_BLE_MANUFACTURER
//#define MICROBIT_BLE_MAXIMUM_BONDS
//#define MICROBIT_BLE_MAXIMUM_SCROLLTEXT
//#define MICROBIT_BLE_MODEL
//#define MICROBIT_BLE_OPEN
//#define MICROBIT_BLE_PAIR_COMPLETE
//#define MICROBIT_BLE_PAIRING_MODE
//#define MICROBIT_BLE_PAIRING_TIMEOUT
//#define MICROBIT_BLE_PAIR_PASSCODE
//#define MICROBIT_BLE_PAIR_REQUEST
//#define MICROBIT_BLE_PAIR_SUCCESSFUL
//#define MICROBIT_BLE_PRIVATE_ADDRESSES
//#define MICROBIT_BLE_SECURITY_LEVEL
//#define MICROBIT_BLE_SOFTWARE_VERSION
//#define MICROBIT_BLE_STATUS_DISCONNECT
//#define MICROBIT_BLE_STATUS_STORE_SYSATTR
//#define MICROBIT_BLE_WHITELIST

//#define MICROBIT_DFU_HISTOGRAM_HEIGHT
//#define MICROBIT_DFU_HISTOGRAM_WIDTH
//#define MICROBIT_DFU_OPCODE_START_DFU

//#define MICROBIT_HEAP_REUSE_SD
//#define MICROBIT_SD_GATT_TABLE_SIZE
//#define MICROBIT_SD_GATT_TABLE_START
//#define MICROBIT_SD_LIMIT
//#define MICROBIT_ID_BLE
//#define MICROBIT_ID_BLE_UART
//#define MICROBIT_IO_PIN_SERVICE_DATA_SIZE
//#define MICROBIT_IO_PIN_SERVICE_PINCOUNT

//#define MICROBIT_NAME_CODE_LETTERS
//#define MICROBIT_NAME_LENGTH
//#define MICROBIT_PAIRING_FADE_SPEED
//#define MICROBIT_PWM_PIN_SERVICE_DATA_SIZE

//#define MICROBIT_UART_S_DEFAULT_BUF_SIZE
//#define MICROBIT_UART_S_EVT_DELIM_MATCH
//#define MICROBIT_UART_S_EVT_HEAD_MATCH
//#define MICROBIT_UART_S_EVT_RX_FULL
#define MICROBIT_UART_S_EVT_TX_EMPTY                    BLE_EVT_SERIAL_TX_EMPTY

//
// Light Sensor not yet implemented
//

//#define MICROBIT_LIGHT_SENSOR_AN_SET_TIME
//#define MICROBIT_LIGHT_SENSOR_CHAN_NUM
//#define MICROBIT_LIGHT_SENSOR_MAX_VALUE
//#define MICROBIT_LIGHT_SENSOR_MIN_VALUE
//#define MICROBIT_LIGHT_SENSOR_TICK_PERIOD
//#define MICROBIT_LIGHT_SENSOR_TICK_SPEED

//
// Storage not yet implemented
//

//#define MICROBIT_STORAGE_BLOCK_SIZE
//#define MICROBIT_STORAGE_KEY_SIZE
//#define MICROBIT_STORAGE_MAGIC
//#define MICROBIT_STORAGE_SCRATCH_PAGE_OFFSET
//#define MICROBIT_STORAGE_STORE_PAGE_OFFSET
//#define MICROBIT_STORAGE_VALUE_SIZE


//
// MicroBit Panic not yet implemented
//

//#define MICROBIT_PANIC_ERROR_CHARS


//
// MICROBIT specific component IDs and event codes
//

#define MICROBIT_ACCELEROMETER_EVT_3G                           ACCELEROMETER_EVT_3G
#define MICROBIT_ACCELEROMETER_EVT_6G                           ACCELEROMETER_EVT_6G
#define MICROBIT_ACCELEROMETER_EVT_8G                           ACCELEROMETER_EVT_8G
#define MICROBIT_ACCELEROMETER_EVT_DATA_UPDATE                  ACCELEROMETER_EVT_DATA_UPDATE
#define MICROBIT_ACCELEROMETER_EVT_FACE_DOWN                    ACCELEROMETER_EVT_FACE_DOWN
#define MICROBIT_ACCELEROMETER_EVT_FACE_UP                      ACCELEROMETER_EVT_FACE_UP
#define MICROBIT_ACCELEROMETER_EVT_FREEFALL                     ACCELEROMETER_EVT_FREEFALL
#define MICROBIT_ACCELEROMETER_EVT_NONE                         ACCELEROMETER_EVT_NONE
#define MICROBIT_ACCELEROMETER_EVT_SHAKE                        ACCELEROMETER_EVT_SHAKE
#define MICROBIT_ACCELEROMETER_EVT_TILT_DOWN                    ACCELEROMETER_EVT_TILT_DOWN
#define MICROBIT_ACCELEROMETER_EVT_TILT_LEFT                    ACCELEROMETER_EVT_TILT_LEFT
#define MICROBIT_ACCELEROMETER_EVT_TILT_RIGHT                   ACCELEROMETER_EVT_TILT_RIGHT
#define MICROBIT_ACCELEROMETER_EVT_TILT_UP                      ACCELEROMETER_EVT_TILT_UP

#define MICROBIT_BUTTON_ALL_EVENTS                              DEVICE_BUTTON_ALL_EVENTS
#define MICROBIT_BUTTON_SIMPLE_EVENTS                           DEVICE_BUTTON_SIMPLE_EVENTS
#define MICROBIT_BUTTON_EVT_CLICK                               DEVICE_BUTTON_EVT_CLICK
#define MICROBIT_BUTTON_EVT_DOUBLE_CLICK                        DEVICE_BUTTON_EVT_DOUBLE_CLICK
#define MICROBIT_BUTTON_EVT_DOWN                                DEVICE_BUTTON_EVT_DOWN
#define MICROBIT_BUTTON_EVT_HOLD                                DEVICE_BUTTON_EVT_HOLD
#define MICROBIT_BUTTON_EVT_LONG_CLICK                          DEVICE_BUTTON_EVT_LONG_CLICK
#define MICROBIT_BUTTON_EVT_UP                                  DEVICE_BUTTON_EVT_UP

#define MICROBIT_COMPASS_EVT_CAL_END                            COMPASS_EVT_CAL_END
#define MICROBIT_COMPASS_EVT_CALIBRATE                          COMPASS_EVT_CALIBRATE
#define MICROBIT_COMPASS_EVT_CAL_REQUIRED                       COMPASS_EVT_CAL_REQUIRED
#define MICROBIT_COMPASS_EVT_CAL_START                          COMPASS_EVT_CAL_START
#define MICROBIT_COMPASS_EVT_CONFIG_NEEDED                      COMPASS_EVT_CONFIG_NEEDED
#define MICROBIT_COMPASS_EVT_DATA_UPDATE                        COMPASS_EVT_DATA_UPDATE
#define MICROBIT_COMPASS_EVT_CALIBRATION_NEEDED                 COMPASS_EVT_CALIBRATION_NEEDED

#define MICROBIT_DISPLAY_EVT_ANIMATION_COMPLETE                 DISPLAY_EVT_ANIMATION_COMPLETE
#define MICROBIT_DISPLAY_EVT_FREE                               DISPLAY_EVT_FREE
#define MICROBIT_DISPLAY_EVT_LIGHT_SENSE                        DISPLAY_EVT_LIGHT_SENSE

#define MICROBIT_SERIAL_EVT_DELIM_MATCH                         CODAL_SERIAL_EVT_DELIM_MATCH
#define MICROBIT_SERIAL_EVT_HEAD_MATCH                          CODAL_SERIAL_EVT_HEAD_MATCH
#define MICROBIT_SERIAL_EVT_RX_FULL                             CODAL_SERIAL_EVT_RX_FULL
#define MICROBIT_SERIAL_EVT_TX_EMPTY                            CODAL_SERIAL_EVT_TX_EMPTY

#define MICROBIT_ID_ANY                                         DEVICE_ID_ANY
#define MICROBIT_EVT_ANY                                        DEVICE_EVT_ANY

#define MICROBIT_ID_ACCELEROMETER                               DEVICE_ID_ACCELEROMETER
#define MICROBIT_ID_BUTTON_A                                    DEVICE_ID_BUTTON_A
#define MICROBIT_ID_BUTTON_B                                    DEVICE_ID_BUTTON_B
#define MICROBIT_ID_BUTTON_AB                                   DEVICE_ID_BUTTON_AB
#define MICROBIT_ID_BUTTON_RESET                                DEVICE_ID_BUTTON_RESET
#define MICROBIT_ID_COMPASS                                     DEVICE_ID_COMPASS
#define MICROBIT_ID_DISPLAY                                     DEVICE_ID_DISPLAY
#define MICROBIT_ID_GESTURE                                     DEVICE_ID_GESTURE
#define MICROBIT_ID_IO_P0                                       ID_PIN_P0
#define MICROBIT_ID_IO_P1                                       ID_PIN_P1
#define MICROBIT_ID_IO_P2                                       ID_PIN_P2
#define MICROBIT_ID_IO_P3                                       ID_PIN_P3
#define MICROBIT_ID_IO_P4                                       ID_PIN_P4
#define MICROBIT_ID_IO_P5                                       ID_PIN_P5
#define MICROBIT_ID_IO_P6                                       ID_PIN_P6
#define MICROBIT_ID_IO_P7                                       ID_PIN_P7
#define MICROBIT_ID_IO_P8                                       ID_PIN_P8
#define MICROBIT_ID_IO_P9                                       ID_PIN_P9
#define MICROBIT_ID_IO_P10                                      ID_PIN_P10
#define MICROBIT_ID_IO_P11                                      ID_PIN_P11
#define MICROBIT_ID_IO_P12                                      ID_PIN_P12
#define MICROBIT_ID_IO_P13                                      ID_PIN_P13
#define MICROBIT_ID_IO_P14                                      ID_PIN_P14
#define MICROBIT_ID_IO_P15                                      ID_PIN_P15
#define MICROBIT_ID_IO_P16                                      ID_PIN_P16
#define MICROBIT_ID_IO_P19                                      ID_PIN_P19
#define MICROBIT_ID_IO_P20                                      ID_PIN_P20
#define MICROBIT_ID_MESSAGE_BUS_LISTENER                        DEVICE_ID_MESSAGE_BUS_LISTENER
#define MICROBIT_ID_MULTIBUTTON_ATTACH                          DEVICE_ID_MULTIBUTTON_ATTACH
#define MICROBIT_ID_NOTIFY                                      DEVICE_ID_NOTIFY
#define MICROBIT_ID_NOTIFY_ONE                                  DEVICE_ID_NOTIFY_ONE
#define MICROBIT_ID_RADIO                                       DEVICE_ID_RADIO
#define MICROBIT_ID_RADIO_DATA_READY                            DEVICE_ID_RADIO_DATA_READY
#define MICROBIT_ID_SERIAL                                      DEVICE_ID_SERIAL
#define MICROBIT_ID_THERMOMETER                                 DEVICE_ID_THERMOMETER

#define MICROBIT_ID_PARTIAL_FLASHING                            36

#define MICROBIT_MAXIMUM_HEAPS                                  DEVICE_MAXIMUM_HEAPS
#define MICROBIT_NESTED_HEAP_SIZE                               0
#define MICROBIT_PANIC_HEAP_FULL                                DEVICE_PANIC_HEAP_FULL

#define MICROBIT_SCHEDULER_RUNNING                              DEVICE_SCHEDULER_RUNNING
#define MICROBIT_SERIAL_DEFAULT_BAUD_RATE                       CODAL_SERIAL_DEFAULT_BAUD_RATE
#define MICROBIT_SERIAL_DEFAULT_BUFFER_SIZE                     CODAL_SERIAL_DEFAULT_BUFFER_SIZE
#define MICROBIT_SERIAL_RX_BUFF_INIT                            CODAL_SERIAL_RX_BUFF_INIT
#define MICROBIT_SERIAL_RX_IN_USE                               CODAL_SERIAL_RX_IN_USE
#define MICROBIT_SERIAL_TX_BUFF_INIT                            CODAL_SERIAL_TX_BUFF_INIT
#define MICROBIT_SERIAL_TX_IN_USE                               CODAL_SERIAL_TX_IN_USE
#define MICROBIT_SRAM_BASE                                      DEVICE_SRAM_BASE
#define MICROBIT_SRAM_END                                       DEVICE_SRAM_END
#define MICROBIT_STACK_SIZE                                     DEVICE_STACK_SIZE

#define SCK uBit.io.P13
#define MISO uBit.io.P14
#define MOSI uBit.io.P15
#define SPI NRF52SPI

#define MICROBIT_ACCEL_ADDED_TO_IDLE                            COMPASS_STATUS_ADDED_TO_IDLE
#define MICROBIT_COMPASS_STATUS_ADDED_TO_IDLE                   COMPASS_STATUS_ADDED_TO_IDLE

namespace codal
{
    void fiber_add_idle_component(CodalComponent *c);
};

using namespace codal;

#endif
