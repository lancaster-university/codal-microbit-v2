/*
The MIT License (MIT)

Copyright (c) 2020 Lancaster University.

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

#include "MicroBitSerial.h"

using namespace codal;

/**
 * Constructor.
 * Create an instance of DeviceSerial
 *
 * @param tx the Pin to be used for transmission
 *
 * @param rx the Pin to be used for receiving data
 *
 * @param rxBufferSize the size of the buffer to be used for receiving bytes
 *
 * @param txBufferSize the size of the buffer to be used for transmitting bytes
 *
 * @code
 * DeviceSerial serial(USBTX, USBRX);
 * @endcode
 * @note the default baud rate is 115200. More API details can be found:
 *       -https://github.com/mbedmicro/mbed/blob/master/libraries/mbed/api/SerialBase.h
 *       -https://github.com/mbedmicro/mbed/blob/master/libraries/mbed/api/RawSerial.h
 *
 *       Buffers aren't allocated until the first send or receive respectively.
 */
MicroBitSerial::MicroBitSerial(Pin& tx, Pin& rx, uint8_t rxBufferSize, uint8_t txBufferSize, uint16_t id) : NRF52Serial(tx, rx)
{
}

/**
 * Constructor.
 * Create an instance of DeviceSerial
 *
 * @param tx the Pin to be used for transmission
 *
 * @param rx the Pin to be used for receiving data
 *
 * @param rxBufferSize the size of the buffer to be used for receiving bytes
 *
 * @param txBufferSize the size of the buffer to be used for transmitting bytes
 *
 * @code
 * DeviceSerial serial(USBTX, USBRX);
 * @endcode
 * @note the default baud rate is 115200. More API details can be found:
 *       -https://github.com/mbedmicro/mbed/blob/master/libraries/mbed/api/SerialBase.h
 *       -https://github.com/mbedmicro/mbed/blob/master/libraries/mbed/api/RawSerial.h
 *
 *       Buffers aren't allocated until the first send or receive respectively.
 */
MicroBitSerial::MicroBitSerial(PinName tx, PinName rx, uint8_t rxBufferSize, uint8_t txBufferSize, uint16_t id) : NRF52Serial(*new NRF52Pin(tx, tx, PIN_CAPABILITY_ALL), *new NRF52Pin(rx, rx, PIN_CAPABILITY_ALL))
{
}


/**
 * Constructor.
 * Create an instance of DeviceSerial
 *
 * @param tx the Pin to be used for transmission
 *
 * @param rx the Pin to be used for receiving data
 *
 * @param rxBufferSize the size of the buffer to be used for receiving bytes
 *
 * @param txBufferSize the size of the buffer to be used for transmitting bytes
 *
 * @code
 * DeviceSerial serial(USBTX, USBRX);
 * @endcode
 * @note the default baud rate is 115200. More API details can be found:
 *       -https://github.com/mbedmicro/mbed/blob/master/libraries/mbed/api/SerialBase.h
 *       -https://github.com/mbedmicro/mbed/blob/master/libraries/mbed/api/RawSerial.h
 *
 *       Buffers aren't allocated until the first send or receive respectively.
 */
MicroBitSerial::MicroBitSerial(PinNumber tx, PinNumber rx, uint8_t rxBufferSize, uint8_t txBufferSize, uint16_t id) : NRF52Serial(*new NRF52Pin(tx, tx, PIN_CAPABILITY_ALL), *new NRF52Pin(rx, rx, PIN_CAPABILITY_ALL))
{
}

/**
  * A way of dynamically configuring the serial instance to use pins other than USBTX and USBRX.
  *
  * @param tx the new transmission pin.
  *
  * @param rx the new reception pin.
  *
  * @return CODAL_SERIAL_IN_USE if another fiber is currently transmitting or receiving, otherwise DEVICE_OK.
  */
int MicroBitSerial::redirect(PinName tx, PinName rx) {
    static Pin *oldRx = NULL;
    static Pin *oldTx = NULL;

    if (oldRx)
        delete oldRx;

    if (oldTx)
        delete oldTx;

    oldTx = new Pin(tx, tx, PIN_CAPABILITY_ALL);
    oldRx = new Pin(rx, rx, PIN_CAPABILITY_ALL);

    return Serial::redirect(*oldTx, *oldRx);
}

/**
  * A way of dynamically configuring the serial instance to use pins other than USBTX and USBRX.
  *
  * @param tx the new transmission pin.
  *
  * @param rx the new reception pin.
  *
  * @return CODAL_SERIAL_IN_USE if another fiber is currently transmitting or receiving, otherwise DEVICE_OK.
  */
int MicroBitSerial::redirect(PinNumber tx, PinNumber rx) {
    static Pin *oldRx = NULL;
    static Pin *oldTx = NULL;

    if (oldRx)
        delete oldRx;

    if (oldTx)
        delete oldTx;

    oldTx = new Pin(tx, tx, PIN_CAPABILITY_ALL);
    oldRx = new Pin(rx, rx, PIN_CAPABILITY_ALL);

    return Serial::redirect(*oldTx, *oldRx);
}
