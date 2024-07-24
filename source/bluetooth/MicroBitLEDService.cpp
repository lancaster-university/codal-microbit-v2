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

/**
  * Class definition for the custom MicroBit LED Service.
  * Provides a BLE service to remotely read and write the state of the LED display.
  */
#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitLEDService.h"

using namespace codal;

const uint16_t MicroBitLEDService::serviceUUID               = 0xd91d;
const uint16_t MicroBitLEDService::charUUID[ mbbs_cIdxCOUNT] = { 0x7b77, 0x93ee, 0x0d2d };

/**
  * Constructor.
  * Create a representation of the LEDService
  * @param _ble The instance of a BLE device that we're running on.
  * @param _display An instance of MicroBitDisplay to interface with.
  */
MicroBitLEDService::MicroBitLEDService( BLEDevice &_ble, MicroBitDisplay &_display) :
    display(_display)
{
    // Initialise our characteristic values.
    memclr( matrixValue, sizeof( matrixValue));
    textValue[0]    = 0;
    speedValue      = MICROBIT_DEFAULT_SCROLL_SPEED;
    
    // Register the base UUID and create the service.
    RegisterBaseUUID( bs_base_uuid);
    CreateService( serviceUUID);
    
    // Add each of our characteristics.
    CreateCharacteristic( mbbs_cIdxMATRIX, charUUID[ mbbs_cIdxMATRIX],
                         (uint8_t *)matrixValue,
                         sizeof(matrixValue), sizeof(matrixValue),
                         microbit_propWRITE | microbit_propREAD | microbit_propREADAUTH);
    
    CreateCharacteristic( mbbs_cIdxTEXT,   charUUID[ mbbs_cIdxTEXT],
                         (uint8_t *)textValue,
                         sizeof(textValue), sizeof(textValue),
                         microbit_propWRITE);
    
    CreateCharacteristic( mbbs_cIdxSPEED,  charUUID[ mbbs_cIdxSPEED],
                         (uint8_t *)&speedValue,
                         sizeof(speedValue), sizeof(speedValue),
                         microbit_propWRITE | microbit_propREAD);
}


/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitLEDService::onDataWritten( const microbit_ble_evt_write_t *params)
{
    uint8_t *data = (uint8_t *)params->data;

    if (params->handle == valueHandle( mbbs_cIdxMATRIX) && params->len > 0 && params->len < 6)
    {
       // interrupt any animation that might be currently going on
       display.stopAnimation();
       for (int y=0; y<params->len; y++)
            for (int x=0; x<5; x++)
                display.image.setPixelValue(x, y, (data[y] & (0x01 << (4-x))) ? 255 : 0);
    }

    else if (params->handle == valueHandle( mbbs_cIdxTEXT))
    {
        // Create a ManagedString representation from the UTF8 data.
        // We do this explicitly to control the length (in case the string is not NULL terminated!)
        ManagedString s((char *)params->data, params->len);

        // interrupt any animation that might be currently going on
        display.stopAnimation();

        // Start the string scrolling and we're done.
        display.scrollAsync(s, (int) speedValue);
    }

    else if (params->handle == valueHandle( mbbs_cIdxSPEED) && params->len >= sizeof(speedValue))
    {
        // Read the speed requested, and store it locally.
        // We use this as the speed for all scroll operations subsquently initiated from BLE.
        memcpy(&speedValue, params->data, sizeof(speedValue));
    }
}


/**
  * Callback. Invoked when any of our attributes are read via BLE.
  * Set  params->data and params->length to update the value
  */
void MicroBitLEDService::onDataRead( microbit_onDataRead_t *params)
{
    if ( params->handle == valueHandle( mbbs_cIdxMATRIX))
    {
        for (int y=0; y<5; y++)
        {
            matrixValue[y] = 0;

            for (int x=0; x<5; x++)
            {
                if (display.image.getPixelValue(x, y))
                    matrixValue[y] |= 0x01 << (4-x);
            }
        }

        params->data    = matrixValue;
        params->length  = sizeof(matrixValue);
    }
}

#endif
