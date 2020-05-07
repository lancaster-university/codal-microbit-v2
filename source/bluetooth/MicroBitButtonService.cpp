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
  * Class definition for the custom MicroBit Button Service.
  * Provides a BLE service to remotely read the state of each button, and configure its behaviour.
  */
#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitButtonService.h"
#include "MicroBitButton.h"

const uint16_t MicroBitButtonService::serviceUUID               = 0x9882;
const uint16_t MicroBitButtonService::charUUID[ mbbs_cIdxCOUNT] = { 0xda90, 0xda91 };

/**
  * Constructor.
  * Create a representation of the ButtonService
  * @param _ble The instance of a BLE device that we're running on.
  */
MicroBitButtonService::MicroBitButtonService( BLEDevice &_ble)
{
    // Initialise our characteristic values.
    buttonADataCharacteristicBuffer = 0;
    buttonBDataCharacteristicBuffer = 0;
    
    // Register the base UUID and create the service.
    RegisterBaseUUID( bs_base_uuid);
    CreateService( serviceUUID);

    // Create the data structures that represent each of our characteristics in Soft Device.
    CreateCharacteristic( mbbs_cIdxA, charUUID[ mbbs_cIdxA],
                         (uint8_t *)&buttonADataCharacteristicBuffer,
                         sizeof(buttonADataCharacteristicBuffer), sizeof(buttonADataCharacteristicBuffer),
                         microbit_propREAD | microbit_propNOTIFY);

    CreateCharacteristic( mbbs_cIdxB, charUUID[ mbbs_cIdxB],
                         (uint8_t *)&buttonBDataCharacteristicBuffer,
                         sizeof(buttonBDataCharacteristicBuffer), sizeof(buttonBDataCharacteristicBuffer),
                         microbit_propREAD | microbit_propNOTIFY);

    if ( getConnected())
        listen( true);
}


/**
  * Set up or tear down event listers
  */
void MicroBitButtonService::listen( bool yes)
{
    if (EventModel::defaultEventBus)
    {
        if ( yes)
        {
            // Using MESSAGE_BUS_LISTENER_IMMEDIATE results in a crash in notify()
            EventModel::defaultEventBus->listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, this, &MicroBitButtonService::buttonAUpdate);
            EventModel::defaultEventBus->listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, this, &MicroBitButtonService::buttonBUpdate);
        }
        else
        {
            EventModel::defaultEventBus->ignore(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, this, &MicroBitButtonService::buttonAUpdate);
            EventModel::defaultEventBus->ignore(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, this, &MicroBitButtonService::buttonBUpdate);
        }
    }
}


/**
  * Invoked when BLE connects.
  */
void MicroBitButtonService::onConnect( const microbit_ble_evt_t *p_ble_evt)
{
    listen( true);
}


/**
  * Invoked when BLE disconnects.
  */
void MicroBitButtonService::onDisconnect( const microbit_ble_evt_t *p_ble_evt)
{
    listen( false);
}


/**
  * Button A update callback
  */
void MicroBitButtonService::buttonAUpdate(MicroBitEvent e)
{
    if ( getConnected())
    {
        if (e.value == MICROBIT_BUTTON_EVT_UP)
        {
            buttonADataCharacteristicBuffer = 0;
            notifyChrValue( mbbs_cIdxA, &buttonADataCharacteristicBuffer, sizeof(buttonADataCharacteristicBuffer));
        }

        if (e.value == MICROBIT_BUTTON_EVT_DOWN)
        {
            buttonADataCharacteristicBuffer = 1;
            notifyChrValue( mbbs_cIdxA, &buttonADataCharacteristicBuffer, sizeof(buttonADataCharacteristicBuffer));
        }

        if (e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            buttonADataCharacteristicBuffer = 2;
            notifyChrValue( mbbs_cIdxA, &buttonADataCharacteristicBuffer, sizeof(buttonADataCharacteristicBuffer));
        }
    }
}

/**
  * Button B update callback
  */
void MicroBitButtonService::buttonBUpdate(MicroBitEvent e)
{
    if ( getConnected())
    {
        if (e.value == MICROBIT_BUTTON_EVT_UP)
        {
            buttonBDataCharacteristicBuffer = 0;
            notifyChrValue( mbbs_cIdxB, &buttonBDataCharacteristicBuffer, sizeof(buttonBDataCharacteristicBuffer));
        }

        if (e.value == MICROBIT_BUTTON_EVT_DOWN)
        {
            buttonBDataCharacteristicBuffer = 1;
            notifyChrValue( mbbs_cIdxB, &buttonBDataCharacteristicBuffer, sizeof(buttonBDataCharacteristicBuffer));
        }

        if (e.value == MICROBIT_BUTTON_EVT_HOLD)
        {
            buttonBDataCharacteristicBuffer = 2;
            notifyChrValue( mbbs_cIdxB, &buttonBDataCharacteristicBuffer, sizeof(buttonBDataCharacteristicBuffer));
        }
    }
}

#endif
