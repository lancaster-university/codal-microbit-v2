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

#ifndef MICROBIT_I2C_H
#define MICROBIT_I2C_H

#include "MicroBitCompat.h"

namespace codal
{

/**
  * Class definition for MicroBit I2C
  *
  */
class MicroBitI2C : public NRF52I2C
{
    public:
    /**
      * Constructor.
      *
      * @param sda the physical pin on the processor that should be used as output.
      *
      * @param scl the physical pin on the processor that should be used as output.
      *
      * @param device
      */
     MicroBitI2C(NRF52Pin &sda, NRF52Pin &scl);

    /**
      * Constructor.
      *
      * @param sda the physical pin on the processor that should be used as output.
      *
      * @param scl the physical pin on the processor that should be used as output.
      *
      * @param device
      */
     MicroBitI2C(PinName sda, PinName scl);
    
     /**
      * Constructor.
      *
      * @param sda the physical pin on the processor that should be used as output.
      *
      * @param scl the physical pin on the processor that should be used as output.
      *
      * @param device
      */
     MicroBitI2C(PinNumber sda, PinNumber scl);

};

}

#endif
