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

#ifndef MICROBIT_BLE_CHAR_H
#define MICROBIT_BLE_CHAR_H

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitBLETypes.h"

namespace codal
{

/**
  * Class definition for MicroBitBLEChar.
  * Provides a base class for the BLE sevices.
  */
class MicroBitBLEChar
{
    public:
    
    /**
      * Constructor.
      * Create a representation of the BLE Characteristic
      */
    MicroBitBLEChar();

    public:
    
    bool setChrValue(       microbit_gaphandle_t connection, const uint8_t *data, uint16_t length);
    bool notifyChrValue(    microbit_gaphandle_t connection, const uint8_t *data, uint16_t length);
    bool indicateChrValue(  microbit_gaphandle_t connection, const uint8_t *data, uint16_t length);
    bool writeChrValue(     microbit_gaphandle_t connection, const uint8_t *data, uint16_t length);

    microbit_charhandles_t  *charHandles() { return &handles; }
    
    bool cccdNotify()   { return cccd & BLE_GATT_HVX_NOTIFICATION; }
    bool cccdIndicate() { return cccd & BLE_GATT_HVX_INDICATION; }

    void setCCCD(  uint16_t c) { cccd  = c; }
    
    private:
    
    microbit_charhandles_t  handles;
    uint16_t                cccd;
};

} // namespace codal

#endif // CONFIG_ENABLED(DEVICE_BLE)
#endif // MICROBIT_BLE_CHAR_H
