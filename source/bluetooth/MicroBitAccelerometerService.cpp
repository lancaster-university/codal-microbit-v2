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
  * Class definition for the custom MicroBit Accelerometer Service.
  * Provides a BLE service to remotely read the state of the accelerometer, and configure its behaviour.
  */
#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitAccelerometerService.h"


const uint16_t MicroBitAccelerometerService::serviceUUID               = 0x0753;
const uint16_t MicroBitAccelerometerService::charUUID[ mbbs_cIdxCOUNT] = { 0xca4b, 0xfb24 };


/**
  * Constructor.
  * Create a representation of the AccelerometerService
  * @param _ble The instance of a BLE device that we're running on.
  * @param _accelerometer An instance of MicroBitAccelerometer.
  */
MicroBitAccelerometerService::MicroBitAccelerometerService( BLEDevice &_ble, MicroBitAccelerometer &_accelerometer) :
        accelerometer(_accelerometer)
{
    // Initialise our characteristic values.
    accelerometerDataCharacteristicBuffer[0] = 0;
    accelerometerDataCharacteristicBuffer[1] = 0;
    accelerometerDataCharacteristicBuffer[2] = 0;
    accelerometerPeriodCharacteristicBuffer = 0;

    // Register the base UUID and create the service.
    RegisterBaseUUID( bs_base_uuid);
    CreateService( serviceUUID);
    
    // Create the data structures that represent each of our characteristics in Soft Device.
    CreateCharacteristic( mbbs_cIdxDATA, charUUID[ mbbs_cIdxDATA],
                         (uint8_t *)accelerometerDataCharacteristicBuffer,
                         sizeof(accelerometerDataCharacteristicBuffer), sizeof(accelerometerDataCharacteristicBuffer),
                         microbit_propREAD | microbit_propNOTIFY);

    CreateCharacteristic( mbbs_cIdxPERIOD, charUUID[ mbbs_cIdxPERIOD],
                         (uint8_t *)&accelerometerPeriodCharacteristicBuffer,
                         sizeof(accelerometerPeriodCharacteristicBuffer), sizeof(accelerometerPeriodCharacteristicBuffer),
                         microbit_propREAD | microbit_propWRITE);

    if ( getConnected())
        listen( true);
}


void MicroBitAccelerometerService::readXYZ()
{
    accelerometerDataCharacteristicBuffer[0] = accelerometer.getX();
    accelerometerDataCharacteristicBuffer[1] = accelerometer.getY();
    accelerometerDataCharacteristicBuffer[2] = accelerometer.getZ();
}


/**
  * Set up or tear down event listers
  */
void MicroBitAccelerometerService::listen( bool yes)
{
    if (EventModel::defaultEventBus)
    {
        if ( yes)
        {
            // Ensure accelerometer is being updated
            readXYZ();
            accelerometerPeriodCharacteristicBuffer = accelerometer.getPeriod();
            EventModel::defaultEventBus->listen(MICROBIT_ID_ACCELEROMETER, MICROBIT_ACCELEROMETER_EVT_DATA_UPDATE, this, &MicroBitAccelerometerService::accelerometerUpdate, MESSAGE_BUS_LISTENER_IMMEDIATE);
        }
        else
        {
            EventModel::defaultEventBus->ignore(MICROBIT_ID_ACCELEROMETER, MICROBIT_ACCELEROMETER_EVT_DATA_UPDATE, this, &MicroBitAccelerometerService::accelerometerUpdate);
        }
    }
}


/**
  * Invoked when BLE connects.
  */
void MicroBitAccelerometerService::onConnect( const microbit_ble_evt_t *p_ble_evt)
{
    listen( true);
}


/**
  * Invoked when BLE disconnects.
  */
void MicroBitAccelerometerService::onDisconnect( const microbit_ble_evt_t *p_ble_evt)
{
    listen( false);
}


/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitAccelerometerService::onDataWritten( const microbit_ble_evt_write_t *params)
{
    if (params->handle == valueHandle( mbbs_cIdxPERIOD) && params->len >= sizeof(accelerometerPeriodCharacteristicBuffer))
    {
        accelerometerPeriodCharacteristicBuffer = *((uint16_t *)params->data);
        accelerometer.setPeriod(accelerometerPeriodCharacteristicBuffer);

        // The accelerometer will choose the nearest period to that requested that it can support
        // Read back the ACTUAL period it is using, and store for next read.
        accelerometerPeriodCharacteristicBuffer = accelerometer.getPeriod();
        setChrValue( mbbs_cIdxPERIOD, (const uint8_t *)&accelerometerPeriodCharacteristicBuffer, sizeof(accelerometerPeriodCharacteristicBuffer));
    }
}


/**
  * Accelerometer update callback
  */
void MicroBitAccelerometerService::accelerometerUpdate(MicroBitEvent)
{
    if ( getConnected())
    {
        readXYZ();
        notifyChrValue( mbbs_cIdxDATA, (uint8_t *)accelerometerDataCharacteristicBuffer, sizeof(accelerometerDataCharacteristicBuffer));
    }
}

#endif
