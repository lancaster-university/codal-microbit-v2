/*
The MIT License (MIT)

This class has been written by Sam Kent for the Microbit Educational Foundation.

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

#ifndef MICROBIT_PARTIAL_FLASH_SERVICE_H
#define MICROBIT_PARTIAL_FLASH_SERVICE_H

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitBLEManager.h"
#include "MicroBitBLEService.h"
#include "MicroBitMemoryMap.h"

#include "MicroBitFlash.h"
#include "MicroBitStorage.h"

#include "MicroBitComponent.h"
#include "MicroBitEvent.h"
#include "EventModel.h"

#define PARTIAL_FLASHING_VERSION 0x01

// BLE PF Control Codes
#define REGION_INFO 0x00
#define FLASH_DATA  0x01
#define END_OF_TRANSMISSION 0x02

// BLE Utilities
#define MICROBIT_STATUS 0xEE
#define MICROBIT_RESET  0xFF


/**
  * Class definition for the custom MicroBit Partial Flash Service.
  * Provides a BLE service to remotely read the memory map and flash the PXT program.
  */
class MicroBitPartialFlashingService : public MicroBitBLEService
{
    public:
    /**
      * Constructor.
      * Create a representation of the Partial Flash Service
      * @param _ble The instance of a BLE device that we're running on.
      * @param _messageBus An instance of a MessageBus to interface with.
      * @param _storage A persistent storage manager to use to hold non-volatile state.
      */
    MicroBitPartialFlashingService( BLEDevice &_ble, EventModel &_messageBus, MicroBitStorage &_storage);

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten(const microbit_ble_evt_write_t *params);

    private:
    // MessageBus we're using
    EventModel          &messageBus;
    MicroBitStorage     &storage;

    /**
      * Writing to flash inside MicroBitEvent rather than in the ISR
      */
    void partialFlashingEvent(MicroBitEvent e);

    /**
      * Process a Partial Flashing data packet
      */
    void flashData(uint8_t *data);

    // Ensure packets are in order
    uint8_t packetCount = 0;
    uint8_t blockPacketCount = 0;

    // Keep track of blocks of data
    uint32_t block[16];
    uint8_t  blockNum = 0;
    uint32_t offset   = 0;
    
    uint8_t characteristicValue[ 20];

    // Index for each charactersitic in arrays of handles and UUIDs
    typedef enum mbbs_cIdx
    {
        mbbs_cIdxCTRL,
        mbbs_cIdxCOUNT
    } mbbs_cIdx;
    
    // UUIDs for our service and characteristics
    static const uint8_t  base_uuid[ 16];
    static const uint16_t serviceUUID;
    static const uint16_t charUUID[ mbbs_cIdxCOUNT];
    
    // Data for each characteristic when they are held by Soft Device.
    MicroBitBLEChar      chars[ mbbs_cIdxCOUNT];

    public:
    
    int              characteristicCount()          { return mbbs_cIdxCOUNT; };
    MicroBitBLEChar *characteristicPtr( int idx)    { return &chars[ idx]; };
};


#endif
#endif
