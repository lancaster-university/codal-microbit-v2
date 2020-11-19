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
  * Class definition for the custom MicroBit IOPin Service.
  * Provides a BLE service to remotely read the state of the I/O Pin, and configure its behaviour.
  */
#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitIOPinService.h"
#include "MicroBitFiber.h"

const uint16_t MicroBitIOPinService::serviceUUID               = 0x127b;
const uint16_t MicroBitIOPinService::charUUID[ mbbs_cIdxCOUNT] = { 0x5899, 0xb9fe, 0xd822, 0x8d00 };


/**
  * Constructor.
  * Create a representation of the IOPinService
  * @param _ble The instance of a BLE device that we're running on.
  * @param _io An instance of MicroBitIO that this service will use to perform
  *            I/O operations.
  */
MicroBitIOPinService::MicroBitIOPinService(BLEDevice &_ble, MicroBitIO &_io) :
        io(_io)
{
    // Initialise our characteristic values.
    ioPinServiceADCharacteristicBuffer = 0;
    ioPinServiceIOCharacteristicBuffer = 0;
    memset(ioPinServiceIOData, 0, sizeof(ioPinServiceIOData));
    memset(ioPinServicePWMCharacteristicBuffer, 0, sizeof(ioPinServicePWMCharacteristicBuffer));    // Create the AD characteristic, that defines whether each pin is treated as analogue or digital
    
    // Register the base UUID and create the service.
    RegisterBaseUUID( bs_base_uuid);
    CreateService( serviceUUID);
    
    CreateCharacteristic( mbbs_cIdxADC, charUUID[ mbbs_cIdxADC],
                         (uint8_t *)&ioPinServiceADCharacteristicBuffer,
                         sizeof(ioPinServiceADCharacteristicBuffer), sizeof(ioPinServiceADCharacteristicBuffer),
                         microbit_propREAD | microbit_propWRITE);

    // Create the IO characteristic, that allows the user to define one or more pins as inputs. These will then be monitored by this service and reported via the ioPinSeriveData characterisitic. 
    CreateCharacteristic( mbbs_cIdxIO, charUUID[ mbbs_cIdxIO],
                         (uint8_t *)&ioPinServiceIOCharacteristicBuffer,
                         sizeof(ioPinServiceIOCharacteristicBuffer), sizeof(ioPinServiceIOCharacteristicBuffer),
                         microbit_propREAD | microbit_propWRITE);

    // Create the PWM characteristic, that allows up to 3 compatible pins to be used for PWM
    CreateCharacteristic( mbbs_cIdxPWM, charUUID[ mbbs_cIdxPWM],
                         (uint8_t *)&ioPinServicePWMCharacteristicBuffer,
                         sizeof(ioPinServicePWMCharacteristicBuffer), sizeof(ioPinServicePWMCharacteristicBuffer),
                         microbit_propWRITE);

    // Create the Data characteristic, that allows the actual read and write operations.
    CreateCharacteristic( mbbs_cIdxDATA, charUUID[ mbbs_cIdxDATA],
                         (uint8_t *)ioPinServiceDataCharacteristicBuffer,
                         0, sizeof(ioPinServiceDataCharacteristicBuffer),
                         microbit_propREAD | microbit_propWRITE | microbit_propNOTIFY | microbit_propREADAUTH);

    fiber_add_idle_component(this);
}


MicroBitPin &MicroBitIOPinService::edgePin( int index)
{
    if ( index < 0 || index >= MICROBIT_IO_PIN_SERVICE_PINCOUNT)
        index = 0;
    return io.pin[ index];
};


/**
  * Determines if the given pin was configured as a digital pin by the BLE ADPinConfigurationCharacterisitic.
  *
  * @param i the enumeration of the pin to test
  * @return 1 if this pin is configured as digital, 0 otherwise
  */
int MicroBitIOPinService::isDigital(int i)
{
    return ((ioPinServiceADCharacteristicBuffer & (1 << i)) == 0);
}

/**
  * Determines if the given pin was configured as an analog pin by the BLE ADPinConfigurationCharacterisitic.
  *
  * @param i the enumeration of the pin to test
  * @return 1 if this pin is configured as analog, 0 otherwise
  */
int MicroBitIOPinService::isAnalog(int i)
{
    return ((ioPinServiceADCharacteristicBuffer & (1 << i)) != 0);
}

/**
  * Determines if the given pin was configured as an input by the BLE IOPinConfigurationCharacterisitic.
  *
  * @param i the enumeration of the pin to test
  * @return 1 if this pin is configured as an input, 0 otherwise
  */
int MicroBitIOPinService::isActiveInput(int i)
{
    return ((ioPinServiceIOCharacteristicBuffer & (1 << i)) != 0);
}

/**
 * Scans through all pins that our BLE client have registered an interest in. 
 * For each pin that has changed value, update the BLE characteristic, and NOTIFY our client.
 */
int MicroBitIOPinService::updateBLEInputs(bool updateAll)
{
    int pairs = 0;

    for (int i=0; i < MICROBIT_IO_PIN_SERVICE_PINCOUNT; i++)
    {
        if (isActiveInput(i))
        {
            uint8_t value;

            if (isDigital(i))
               	value = edgePin(i).getDigitalValue();
            else
               	value = edgePin(i).getAnalogValue() >> 2;

            // If the data has changed, send an update.
            if (updateAll || value != ioPinServiceIOData[i])
            {
                ioPinServiceIOData[i] = value;

                ioPinServiceDataCharacteristicBuffer[pairs].pin = i;
                ioPinServiceDataCharacteristicBuffer[pairs].value = value;

                pairs++;

                if (pairs >= MICROBIT_IO_PIN_SERVICE_DATA_SIZE)
                    break;
            }
        }
    }
    
    return pairs;
}

/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitIOPinService::onDataWritten( const microbit_ble_evt_write_t *params)
{
    // Check for writes to the IO configuration characteristic
    if (params->handle == valueHandle( mbbs_cIdxIO) && params->len >= sizeof(ioPinServiceIOCharacteristicBuffer))
    {
        uint32_t *value = (uint32_t *)params->data;

        // Our IO configuration may be changing... read the new value, and push it back into the BLE stack.
        ioPinServiceIOCharacteristicBuffer = *value;
        setChrValue( mbbs_cIdxIO, (const uint8_t *)&ioPinServiceIOCharacteristicBuffer, sizeof(ioPinServiceIOCharacteristicBuffer));

        // Also, drop any selected pins into input mode, so we can pick up changes later
        for (int i=0; i < MICROBIT_IO_PIN_SERVICE_PINCOUNT; i++)
        {
            if(isDigital(i) && isActiveInput(i))
                edgePin(i).getDigitalValue();

            if(isAnalog(i) && isActiveInput(i))
                edgePin(i).getAnalogValue();
        }
    }

    // Check for writes to the IO configuration characteristic
    if (params->handle == valueHandle( mbbs_cIdxADC) && params->len >= sizeof(ioPinServiceADCharacteristicBuffer))
    {
        uint32_t *value = (uint32_t *)params->data;

        // Our IO configuration may be changing... read the new value, and push it back into the BLE stack.
        ioPinServiceADCharacteristicBuffer = *value;
        setChrValue( mbbs_cIdxADC, (const uint8_t *)&ioPinServiceADCharacteristicBuffer, sizeof(ioPinServiceADCharacteristicBuffer));

        // Also, drop any selected pins into input mode, so we can pick up changes later
        for (int i=0; i < MICROBIT_IO_PIN_SERVICE_PINCOUNT; i++)
        {
            if(isDigital(i) && isActiveInput(i))
               edgePin(i).getDigitalValue();

            if(isAnalog(i) && isActiveInput(i))
               edgePin(i).getAnalogValue();
        }
    }

    // Check for writes to the PWM Control characteristic
    if (params->handle == valueHandle( mbbs_cIdxPWM))
    {
        uint16_t len = params->len;
        IOPWMData *pwm_data = (IOPWMData *)params->data;
        
        //validate - len must be a multiple of 7 and greater than 0
        bool is_valid_length = len > 0 && len % 7 == 0;
	    if (is_valid_length)
        {
            uint8_t field_count = len / 7;
            for (int i=0;i<field_count;i++)
            {
                uint8_t  pin    = pwm_data[i].pin;
                uint16_t value = pwm_data[i].value;
                uint32_t period = pwm_data[i].period;
                edgePin(pin).setAnalogValue(value);
                edgePin(pin).setAnalogPeriodUs(period);
            }
        }
        else
        {
            // there's no way to return an error response via the current mbed BLE API :-( See https://github.com/ARMmbed/ble/issues/181
            return;
        }        
    }

    if (params->handle == valueHandle( mbbs_cIdxDATA))
    {
        // We have some pin data to change...
        uint16_t len = params->len;
        IOData *data = (IOData *)params->data;

        // There may be multiple write operations... take each in turn and update the pin values
        while (len >= sizeof(IOData))
        {
            if (!isActiveInput(data->pin))
            {
                if (isDigital(data->pin))
               		edgePin(data->pin).setDigitalValue(data->value);
                else
               		edgePin(data->pin).setAnalogValue(data->value == 255 ? 1023 : data->value << 2);
            }

            data++;
            len -= sizeof(IOData);
        }
    }
}

/**
 * Callback. invoked when the BLE data characteristic is read.
 * Set  params->data and params->length to update the value
 *
 * Reads all the pins marked as inputs, and updates the data stored in the characteristic.
 */
void MicroBitIOPinService::onDataRead( microbit_onDataRead_t *params)
{
    if ( params->handle == valueHandle( mbbs_cIdxDATA))
    {
        int pairs = updateBLEInputs( true);
        if ( pairs)
        {
            params->data    = (uint8_t *)ioPinServiceDataCharacteristicBuffer;
            params->length  = pairs * sizeof(IOData);
        }
    }
}


/**
 * Periodic callback from MicroBit scheduler.
 *
 * Check if any of the pins we're watching need updating. Notify any connected
 * device with any changes.
 */
void MicroBitIOPinService::idleCallback()
{
    if ( getConnected())
    {
        int pairs = updateBLEInputs( false);
        // If there's any data, issue a BLE notification.
        if ( pairs)
        {
            notifyChrValue( mbbs_cIdxDATA, (uint8_t *)ioPinServiceDataCharacteristicBuffer, pairs * sizeof(IOData));
        }
    }
}


#endif
