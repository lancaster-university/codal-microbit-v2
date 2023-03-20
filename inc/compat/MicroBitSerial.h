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

#ifndef MICROBIT_SERIAL_H
#define MICROBIT_SERIAL_H

#include "MicroBitCompat.h"

namespace codal
{

/**
  * Class definition for MicroBit Serial
  *
  */
class MicroBitSerial : public NRF52Serial
{
    public:

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
      * @note the default baud rate is 115200.
      *
      *       Buffers aren't allocated until the first send or receive respectively.
      */
    MicroBitSerial(Pin& tx, Pin& rx, uint8_t rxBufferSize = CODAL_SERIAL_DEFAULT_BUFFER_SIZE, uint8_t txBufferSize = CODAL_SERIAL_DEFAULT_BUFFER_SIZE, uint16_t id  = DEVICE_ID_SERIAL);
    
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
      * @note the default baud rate is 115200.
      *
      *       Buffers aren't allocated until the first send or receive respectively.
      */
    MicroBitSerial(PinName tx, PinName rx, uint8_t rxBufferSize = CODAL_SERIAL_DEFAULT_BUFFER_SIZE, uint8_t txBufferSize = CODAL_SERIAL_DEFAULT_BUFFER_SIZE, uint16_t id  = DEVICE_ID_SERIAL);

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
      * @note the default baud rate is 115200.
      *
      *       Buffers aren't allocated until the first send or receive respectively.
      */
    MicroBitSerial(PinNumber tx, PinNumber rx, uint8_t rxBufferSize = CODAL_SERIAL_DEFAULT_BUFFER_SIZE, uint8_t txBufferSize = CODAL_SERIAL_DEFAULT_BUFFER_SIZE, uint16_t id  = DEVICE_ID_SERIAL);


    /**
      * A way of dynamically configuring the serial instance to use pins other than USBTX and USBRX.
      *
      * @param tx the new transmission pin.
      *
      * @param rx the new reception pin.
      *
      * @return DEVICE_SERIAL_IN_USE if another fiber is currently transmitting or receiving, otherwise DEVICE_OK.
      */
    int redirect(PinName tx, PinName rx);


    /**
      * A way of dynamically configuring the serial instance to use pins other than USBTX and USBRX.
      *
      * @param tx the new transmission pin.
      *
      * @param rx the new reception pin.
      *
      * @return DEVICE_SERIAL_IN_USE if another fiber is currently transmitting or receiving, otherwise DEVICE_OK.
      */
    int redirect(PinNumber tx, PinNumber rx);

};

}
#endif
