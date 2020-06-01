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
  * Class definition for the MicroBit BLE Magnetometer Service.
  * Provides access to live magnetometer data via BLE, and provides basic configuration options.
  */
  
#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitMagnetometerService.h"


const uint16_t MicroBitMagnetometerService::serviceUUID               = 0xf2d8;
const uint16_t MicroBitMagnetometerService::charUUID[ mbbs_cIdxCOUNT] = { 0xfb11, 0x9715, 0x386c, 0xB358 };


/**
  * Constructor.
  * Create a representation of the MagnetometerService.
  * @param _ble The instance of a BLE device that we're running on.
  * @param _compass An instance of MicroBitCompass to use as our Magnetometer source.
  */
MicroBitMagnetometerService::MicroBitMagnetometerService(BLEDevice &_ble, MicroBitCompass &_compass) :
        compass(_compass)
{
    // Initialise our characteristic values.
    magnetometerDataCharacteristicBuffer[0] = 0;
    magnetometerDataCharacteristicBuffer[1] = 0;
    magnetometerDataCharacteristicBuffer[2] = 0;
    magnetometerBearingCharacteristicBuffer = 0;
    magnetometerPeriodCharacteristicBuffer = compass.getPeriod();
    magnetometerCalibrationCharacteristicBuffer = 0;
    
    // Register the base UUID and create the service.
    RegisterBaseUUID( bs_base_uuid);
    CreateService( serviceUUID);
    
    // Create the data structures that represent each of our characteristics in Soft Device.
    CreateCharacteristic( mbbs_cIdxDATA, charUUID[ mbbs_cIdxDATA],
                         (uint8_t *)magnetometerDataCharacteristicBuffer,
                         sizeof(magnetometerDataCharacteristicBuffer), sizeof(magnetometerDataCharacteristicBuffer),
                         microbit_propREAD | microbit_propNOTIFY);

    CreateCharacteristic( mbbs_cIdxBEARING, charUUID[ mbbs_cIdxBEARING],
                         (uint8_t *)&magnetometerBearingCharacteristicBuffer,
                         sizeof(magnetometerBearingCharacteristicBuffer), sizeof(magnetometerBearingCharacteristicBuffer),
                         microbit_propREAD | microbit_propNOTIFY);

    CreateCharacteristic( mbbs_cIdxPERIOD, charUUID[ mbbs_cIdxPERIOD],
                         (uint8_t *)&magnetometerPeriodCharacteristicBuffer,
                         sizeof(magnetometerPeriodCharacteristicBuffer), sizeof(magnetometerPeriodCharacteristicBuffer),
                         microbit_propREAD | microbit_propWRITE);

    CreateCharacteristic( mbbs_cIdxCALIB, charUUID[ mbbs_cIdxCALIB],
                         (uint8_t *)&magnetometerCalibrationCharacteristicBuffer,
                         sizeof(magnetometerCalibrationCharacteristicBuffer), sizeof(magnetometerCalibrationCharacteristicBuffer),
                         microbit_propWRITE | microbit_propNOTIFY);

    if ( getConnected())
        listen( true);
}


/**
 * Fetch magnetometer values to characteristic buffers
 */
void MicroBitMagnetometerService::read()
{
    magnetometerDataCharacteristicBuffer[0] = compass.getX();
    magnetometerDataCharacteristicBuffer[1] = compass.getY();
    magnetometerDataCharacteristicBuffer[2] = compass.getZ();
    magnetometerPeriodCharacteristicBuffer  = compass.getPeriod();

    if ( compass.isCalibrated())
    {
        magnetometerBearingCharacteristicBuffer = (uint16_t) compass.heading();
    }
}


/**
  * Set up or tear down event listers
  */
void MicroBitMagnetometerService::listen( bool yes)
{
    if (EventModel::defaultEventBus)
    {
        if ( yes)
        {
            // Ensure compass is being updated
            read();
            EventModel::defaultEventBus->listen(MICROBIT_ID_COMPASS, MICROBIT_COMPASS_EVT_DATA_UPDATE,        this, &MicroBitMagnetometerService::compassEvents, MESSAGE_BUS_LISTENER_IMMEDIATE);
            EventModel::defaultEventBus->listen(MICROBIT_ID_COMPASS, MICROBIT_COMPASS_EVT_CONFIG_NEEDED,      this, &MicroBitMagnetometerService::compassEvents);
            EventModel::defaultEventBus->listen(MICROBIT_ID_COMPASS, MICROBIT_COMPASS_EVT_CALIBRATION_NEEDED, this, &MicroBitMagnetometerService::compassEvents);
        }
        else
        {
            EventModel::defaultEventBus->ignore(MICROBIT_ID_COMPASS, MICROBIT_EVT_ANY, this, &MicroBitMagnetometerService::compassEvents);
        }
    }
}


/**
  * Invoked when BLE connects.
  */
void MicroBitMagnetometerService::onConnect( const microbit_ble_evt_t *p_ble_evt)
{
    listen( true);
}


/**
  * Invoked when BLE disconnects.
  */
void MicroBitMagnetometerService::onDisconnect( const microbit_ble_evt_t *p_ble_evt)
{
    listen( false);
}


void MicroBitMagnetometerService::calibrateCompass()
{
    int rc = compass.calibrate();
    if (rc == MICROBIT_OK) {
        magnetometerCalibrationCharacteristicBuffer = COMPASS_CALIBRATION_COMPLETED_OK;
    } else {
        magnetometerCalibrationCharacteristicBuffer = COMPASS_CALIBRATION_COMPLETED_ERR;
    }
    notifyChrValue( mbbs_cIdxCALIB, (uint8_t *)&magnetometerCalibrationCharacteristicBuffer, sizeof(magnetometerCalibrationCharacteristicBuffer));
}


void MicroBitMagnetometerService::compassEvents(MicroBitEvent e)
{
    if (e.value == MICROBIT_COMPASS_EVT_DATA_UPDATE) {
        magnetometerUpdate();
        return;
    }
    if (e.value == MICROBIT_COMPASS_EVT_CONFIG_NEEDED) {
        samplePeriodUpdateNeeded();
        return;
    }
    if (e.value == MICROBIT_COMPASS_EVT_CALIBRATION_NEEDED) {
        calibrateCompass();
        return;
    }
}    


/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitMagnetometerService::onDataWritten(const microbit_ble_evt_write_t *params)
{
    if (params->handle == valueHandle( mbbs_cIdxPERIOD) && params->len >= sizeof(magnetometerPeriodCharacteristicBuffer))
    {
        magnetometerPeriodCharacteristicBuffer = *((uint16_t *)params->data);
        MicroBitEvent evt(MICROBIT_ID_COMPASS, MICROBIT_COMPASS_EVT_CONFIG_NEEDED);
        return;
    }

    if (params->handle == valueHandle( mbbs_cIdxCALIB) && params->len >= sizeof(magnetometerCalibrationCharacteristicBuffer))
    {
        magnetometerCalibrationCharacteristicBuffer = *((uint8_t *)params->data);
        if (magnetometerCalibrationCharacteristicBuffer == COMPASS_CALIBRATION_REQUESTED) {
            MicroBitEvent evt(MICROBIT_ID_COMPASS, MICROBIT_COMPASS_EVT_CALIBRATION_NEEDED);
        }
        return;
    }
}


/**
  * Magnetometer update callback
  */
void MicroBitMagnetometerService::magnetometerUpdate()
{
    if ( getConnected())
    {
        read();

        setChrValue( mbbs_cIdxPERIOD, (const uint8_t *)&magnetometerPeriodCharacteristicBuffer, sizeof(magnetometerPeriodCharacteristicBuffer));
        notifyChrValue( mbbs_cIdxDATA,(uint8_t *)magnetometerDataCharacteristicBuffer, sizeof(magnetometerDataCharacteristicBuffer));

        if ( compass.isCalibrated())
        {
            notifyChrValue( mbbs_cIdxBEARING,(uint8_t *)&magnetometerBearingCharacteristicBuffer, sizeof(magnetometerBearingCharacteristicBuffer));
        }
    }
}


/**
 * Sample Period Change Needed callback.
 * Reconfiguring the magnetometer can to a REALLY long time (sometimes even seconds to complete)
 * So we do this in the background when necessary, through this event handler.
 */
void MicroBitMagnetometerService::samplePeriodUpdateNeeded()
{
    // Reconfigure the compass. This might take a while...
    compass.setPeriod(magnetometerPeriodCharacteristicBuffer);

    // The compass will choose the nearest sample period to that we've specified.
    // Read the ACTUAL sample period back.
    magnetometerPeriodCharacteristicBuffer = compass.getPeriod();

    // Ensure this is reflected in our BLE connection.
    setChrValue( mbbs_cIdxPERIOD, (const uint8_t *)&magnetometerPeriodCharacteristicBuffer, sizeof(magnetometerPeriodCharacteristicBuffer));
}


#endif
