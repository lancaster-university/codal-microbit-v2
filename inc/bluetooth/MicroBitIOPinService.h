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

#ifndef MICROBIT_IO_PIN_SERVICE_H
#define MICROBIT_IO_PIN_SERVICE_H

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitBLEManager.h"
#include "MicroBitBLEService.h"
#include "MicroBitIO.h"

#define MICROBIT_IO_PIN_SERVICE_PINCOUNT       19
#define MICROBIT_IO_PIN_SERVICE_DATA_SIZE      10
#define MICROBIT_PWM_PIN_SERVICE_DATA_SIZE     2

namespace codal
{

/**
  * Name value pair definition, as used to read and write pin values over BLE.
  */
struct IOData
{
    uint8_t     pin;
    uint8_t     value;
};

/**
  * PWM control type definition, as used to set PWM properties of one of more conpatible pins over BLE
  */
struct IOPWMData
{
    uint8_t     pin;
    uint16_t    value;
    uint32_t    period;
} __attribute__((packed));

/**
  * Class definition for the custom MicroBit IOPin Service.
  * Provides a BLE service to remotely read the state of the I/O Pin, and configure its behaviour.
  */
class MicroBitIOPinService : public MicroBitBLEService, MicroBitComponent
{
    public:

    /**
      * Constructor.
      * Create a representation of the IOPinService
      * @param _ble The instance of a BLE device that we're running on.
      * @param _io An instance of MicroBitIO that this service will use to perform
      *            I/O operations.
      */
    MicroBitIOPinService(BLEDevice &_ble, MicroBitIO &_io);

    /**
     * Periodic callback from MicroBit scheduler.
     *
     * Check if any of the pins we're watching need updating. Notify any connected
     * device with any changes.
     */
    virtual void idleCallback();

    private:

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten( const microbit_ble_evt_write_t *params);

    /**
     * Callback. invoked when the BLE data characteristic is read.
     *
     * Reads all the pins marked as inputs, and updates the data stored in the characteristic.
     */
    void onDataRead( microbit_onDataRead_t *params);

    /**
      * Determines if the given pin was configured as a digital pin by the BLE ADPinConfigurationCharacterisitic.
      *
      * @param i the enumeration of the pin to test
      * @return 1 if this pin is configured as digital, 0 otherwise
      */
    int isDigital(int i);

    /**
      * Determines if the given pin was configured as an analog pin by the BLE ADPinConfigurationCharacterisitic.
      *
      * @param i the enumeration of the pin to test
      * @return 1 if this pin is configured as analog, 0 otherwise
      */
    int isAnalog(int i);

    /**
      * Determines if the given pin was configured as an input by the BLE IOPinConfigurationCharacterisitic.
      *
      * @param i the enumeration of the pin to test
      * @return 1 if this pin is configured as an input, 0 otherwise
      */
    int isActiveInput(int i);

    /**
     * Scans through all pins that our BLE client have registered an interest in. 
     * For each pin that has changed value, update the BLE characteristic, and NOTIFY our client.
     * @param updateAll if true, a notification will be sent for all registered inputs. Otherwise, 
     * a notification will only be sent for inputs that have changed value.
     * @return number of changed pins
     */
    int updateBLEInputs(bool updateAll = false);

    /**
      * Edge connector pin
      *
      * @param index the enumeration of the pin
      * @return a reference to the pin
      */
    MicroBitPin &edgePin( int index);
    
    // IO we're using
    MicroBitIO          &io;

    // memory for our characteristics.
    uint32_t            ioPinServiceADCharacteristicBuffer;
    uint32_t            ioPinServiceIOCharacteristicBuffer;
    IOPWMData           ioPinServicePWMCharacteristicBuffer[MICROBIT_PWM_PIN_SERVICE_DATA_SIZE];
    IOData              ioPinServiceDataCharacteristicBuffer[MICROBIT_IO_PIN_SERVICE_DATA_SIZE];

    // Historic information about our pin data data.
    uint8_t             ioPinServiceIOData[MICROBIT_IO_PIN_SERVICE_PINCOUNT];
    
    // Index for each charactersitic in arrays of handles and UUIDs
    typedef enum mbbs_cIdx
    {
        mbbs_cIdxADC,
        mbbs_cIdxIO,
        mbbs_cIdxPWM,
        mbbs_cIdxDATA,
        mbbs_cIdxCOUNT
    } mbbs_cIdx;
    
    // UUIDs for our service and characteristics
    static const uint16_t serviceUUID;
    static const uint16_t charUUID[ mbbs_cIdxCOUNT];
    
    // Data for each characteristic when they are held by Soft Device.
    MicroBitBLEChar      chars[ mbbs_cIdxCOUNT];

    public:
    
    int              characteristicCount()          { return mbbs_cIdxCOUNT; };
    MicroBitBLEChar *characteristicPtr( int idx)    { return &chars[ idx]; };
};

} // namespace codal

#endif // CONFIG_ENABLED(DEVICE_BLE)
#endif // MICROBIT_IO_PIN_SERVICE_H
