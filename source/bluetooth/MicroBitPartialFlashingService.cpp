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
  * Class definition for the custom MicroBit Partial Flashing service.
  * Provides a BLE service to remotely write the user program to the device.
  */
#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitPartialFlashingService.h"
#include "MicroBitDevice.h"

#include "nrf_sdm.h"
#include "nrf_dfu_types.h"
#include "crc32.h"

using namespace codal;

const uint8_t  MicroBitPartialFlashingService::base_uuid[ 16] =
{ 0xe9,0x7d,0x00,0x00,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8 };

const uint16_t MicroBitPartialFlashingService::serviceUUID               = 0xd91d;
const uint16_t MicroBitPartialFlashingService::charUUID[ mbbs_cIdxCOUNT] = { 0x3b10 };

uint32_t micropython_fs_start = 0x00;
uint32_t micropython_fs_end   = 0x00;

/**
     * Constructor.
     * Create a representation of the Partial Flash Service
     * @param _ble The instance of a BLE device that we're running on.
     * @param _messageBus An instance of a MessageBus to interface with.
     * @param _storage A persistent storage manager to use to hold non-volatile state.
     */
MicroBitPartialFlashingService::MicroBitPartialFlashingService( BLEDevice &_ble, EventModel &_messageBus, MicroBitStorage &_storage) :
    messageBus(_messageBus), storage(_storage)
{
    // Set up partial flashing characteristic
    memclr( characteristicValue, sizeof( characteristicValue));
    
    // Register the base UUID and create the service.
    RegisterBaseUUID( base_uuid);
    CreateService( serviceUUID);

    CreateCharacteristic( mbbs_cIdxCTRL, charUUID[ mbbs_cIdxCTRL],
                         characteristicValue,
                         sizeof(characteristicValue), sizeof(characteristicValue),
                         microbit_propWRITE_WITHOUT | microbit_propNOTIFY);

    // Set up listener for SD writing
    messageBus.listen( MICROBIT_ID_PARTIAL_FLASHING, MICROBIT_EVT_ANY, this, &MicroBitPartialFlashingService::partialFlashingEvent);
}


/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitPartialFlashingService::onDataWritten(const microbit_ble_evt_write_t *params)
{
    // Get data from BLE callback params
    uint8_t *data = (uint8_t *)params->data;

    if(params->handle == valueHandle( mbbs_cIdxCTRL) && params->len > 0)
    {
      // Switch CONTROL byte
      switch(data[0]){
        case REGION_INFO:
        {
          // Create instance of Memory Map to return info
          MicroBitMemoryMap memoryMap;

          // Get and set MicroPython FS start/end
          if(micropython_fs_end == 0x00 && memoryMap.memoryMapStore.memoryMap[3].startAddress != 0x00) {
            micropython_fs_start = memoryMap.memoryMapStore.memoryMap[3].startAddress;
            micropython_fs_end   = memoryMap.memoryMapStore.memoryMap[3].endAddress;
            DMESG("Size of memory map %x : ", sizeof(memoryMap.memoryMapStore.memoryMap));
          }

          uint8_t buffer[18];
          // Response:
          // Region and Region #
          buffer[0] =  0x00;
          buffer[1] =  data[1];

          // Start Address
          buffer[2] = (memoryMap.memoryMapStore.memoryMap[data[1]].startAddress & 0xFF000000) >> 24;
          buffer[3] = (memoryMap.memoryMapStore.memoryMap[data[1]].startAddress & 0x00FF0000) >> 16;
          buffer[4] = (memoryMap.memoryMapStore.memoryMap[data[1]].startAddress & 0x0000FF00) >>  8;
          buffer[5] = (memoryMap.memoryMapStore.memoryMap[data[1]].startAddress & 0x000000FF);

          // End Address
          buffer[6] = (memoryMap.memoryMapStore.memoryMap[data[1]].endAddress & 0xFF000000) >> 24;
          buffer[7] = (memoryMap.memoryMapStore.memoryMap[data[1]].endAddress & 0x00FF0000) >> 16;
          buffer[8] = (memoryMap.memoryMapStore.memoryMap[data[1]].endAddress & 0x0000FF00) >>  8;
          buffer[9] = (memoryMap.memoryMapStore.memoryMap[data[1]].endAddress & 0x000000FF);

          // Region Hash
          memcpy(&buffer[10], &memoryMap.memoryMapStore.memoryMap[data[1]].hash, 8);

          MICROBIT_DEBUG_DMESGF( "REGION_INFO %x - %x",
            (unsigned int) memoryMap.memoryMapStore.memoryMap[data[1]].startAddress,
            (unsigned int) memoryMap.memoryMapStore.memoryMap[data[1]].endAddress);

          // Send BLE Notification
          notifyChrValue( mbbs_cIdxCTRL, (const uint8_t *)buffer, 18);

          // Reset packet count
          packetCount = 0;
          blockPacketCount = 0;
          blockNum = 0;
          offset = 0;

          break;
        }
        case FLASH_DATA:
        {
          // Process FLASH data packet
          flashData(data);
          break;
        }
        case END_OF_TRANSMISSION:
        {
          /* Start of embedded source isn't always on a page border so client must
           * inform the micro:bit that it's the last packet.
           * - Write final packet
           * The END_OF_TRANSMISSION packet contains no data. Write any data left in the buffer.
           */
           MicroBitEvent evt(MICROBIT_ID_PARTIAL_FLASHING, END_OF_TRANSMISSION);
           break;
        }
        case MICROBIT_STATUS:
        {
          /*
           * Return the version of the Partial Flashing Service and the current BLE mode (application / pairing)
           */
          uint8_t flashNotificationBuffer[] = {MICROBIT_STATUS, PARTIAL_FLASHING_VERSION, MicroBitBLEManager::manager->getCurrentMode()};
          MICROBIT_DEBUG_DMESGF( "MICROBIT_STATUS version %d mode %d", (int)flashNotificationBuffer[1], (int)flashNotificationBuffer[2]);
          notifyChrValue( mbbs_cIdxCTRL, (const uint8_t *)flashNotificationBuffer, sizeof(flashNotificationBuffer));
          break;
        }
        case MICROBIT_RESET:
        {
          /*
           * data[1] determines which mode to reset into: MICROBIT_MODE_PAIRING or MICROBIT_MODE_APPLICATION
           */
           switch(data[1]) {
             case MICROBIT_MODE_PAIRING:
             {
               MICROBIT_DEBUG_DMESGF( "MICROBIT_RESET pairing");
               MicroBitEvent evt(MICROBIT_ID_PARTIAL_FLASHING, MICROBIT_RESET );
               break;
             }
             case MICROBIT_MODE_APPLICATION:
             {
               MICROBIT_DEBUG_DMESGF( "MICROBIT_RESET application");
               microbit_reset();
               break;
             }
           }
           break;
        }
    }
  }
}



/**
  * @param data - A pointer to the data to process
  *
  */
void MicroBitPartialFlashingService::flashData(uint8_t *data)
{
        MICROBIT_DEBUG_DMESGF( "flashData");
        MICROBIT_DEBUG_DMESGF( "  %x %x %x %x", (int) data[0], (int) data[1], (int) data[2], (int) data[3]);
        MICROBIT_DEBUG_DMESGF( "  %x %x %x %x", (int) data[4], (int) data[5], (int) data[6], (int) data[7]);
        MICROBIT_DEBUG_DMESGF( "  %x %x %x %x", (int) data[8], (int) data[9], (int) data[10], (int) data[11]);
        MICROBIT_DEBUG_DMESGF( "  %x %x %x %x", (int) data[12], (int) data[13], (int) data[14], (int) data[15]);
        MICROBIT_DEBUG_DMESGF( "  %x %x %x %x", (int) data[16], (int) data[17], (int) data[18], (int) data[19]);

        // Receive 16 bytes per packet
        // Buffer 4 packets
        // When buffer is full trigger partialFlashingEvent
        // When write is complete notify app and repeat
        // +-----------+---------+---------+----------+
        // | 1 Byte    | 2 Bytes | 1 Byte  | 16 Bytes |
        // +-----------+---------+---------+----------+
        // | COMMAND   | OFFSET  | PACKET# | DATA     |
        // +-----------+---------+---------+----------+
        uint8_t packetNum = data[3];

        MICROBIT_DEBUG_DMESGF( "flashData packetNum %d packetCount %d", (int) packetNum, (int) packetCount);
    
        /**
          * Check packet count
          * If the packet count doesn't match send a notification to the client
          * and set the packet count to the next block number
          */
        if (packetNum != packetCount)
        {
          if ( packetNum < packetCount ? packetCount - packetNum < 8 : packetNum - packetCount > 248 )
            return; // packet is from a previous batch

          MICROBIT_DEBUG_DMESGF( "packet error");
            
          uint8_t flashNotificationBuffer[] = {FLASH_DATA, 0xAA};
          notifyChrValue( mbbs_cIdxCTRL, (const uint8_t *)flashNotificationBuffer, sizeof(flashNotificationBuffer));
          blockPacketCount += 4;
          packetCount = blockPacketCount;
          blockNum = 0;
          return;
        }

        packetCount++;

        // Add to block
        memcpy(block + (4*blockNum), data + 4, 16);

        MICROBIT_DEBUG_DMESG( "blockNum %d", (int) blockNum);
    
        // Actions
        switch(blockNum) {
            // blockNum is 0: set up offset
            case 0:
                {
                    offset = ((data[1] << 8) | data[2] << 0);
                    blockNum++;
                    break;
                }
            // blockNum is 1: complete the offset
            case 1:
                {
                    offset |= ((data[1] << 24) | data[2] << 16);
                    blockNum++;
                    break;
                }
            // blockNum is 3, block is full
            case 3:
                {
                    MICROBIT_DEBUG_DMESG( "Fire write event");
                    // Fire write event
                    MicroBitEvent evt(MICROBIT_ID_PARTIAL_FLASHING, FLASH_DATA );
                    // Reset blockNum
                    blockNum = 0;
                    blockPacketCount += 4;
                    break;
                }
            default:
                {
                    blockNum++;
                    break;
                }
        }

}

/*
 * Set bootloader to no validation
 */
void MicroBitPartialFlashingService::noValidation()
{
    MicroBitFlash flash;

    // make bootloader settings page
    nrf_dfu_settings_t settings;
    memset( &settings, 0, sizeof( nrf_dfu_settings_t));

    // make NO_VALIDATION settings
    settings.settings_version = 2;
    settings.bank_0.bank_code = NRF_DFU_BANK_VALID_APP;
    settings.boot_validation_app.type = NO_VALIDATION;

    // make VALIDATE_CRC settings - calculate app size and CRC
    //settings.boot_validation_app.type = VALIDATE_CRC;
    //settings.bank_0.image_size = offset + 16 * sizeof( uint32_t) - MICROBIT_APP_REGION_START;
    //settings.bank_0.image_crc  = crc32_compute( (uint8_t*) MICROBIT_APP_REGION_START, settings.bank_0.image_size, NULL);
    //memcpy(settings.boot_validation_app.bytes, &settings.bank_0.image_crc, sizeof(uint32_t));

    // calculate settings page CRCs
    settings.crc = crc32_compute( (const uint8_t *)&settings.settings_version,
                                  offsetof(nrf_dfu_settings_t, init_command) - offsetof(nrf_dfu_settings_t, settings_version),
                                  NULL);

    settings.boot_validation_crc = crc32_compute( (const uint8_t *)&settings.boot_validation_softdevice, 3 * sizeof(boot_validation_t), NULL);

    uint32_t settingsSizeInWords = ( sizeof( nrf_dfu_settings_t) + sizeof(uint32_t) - 1) / sizeof(uint32_t);

    // save settings to flash settings page if different
    uint32_t *pSettings = (uint32_t *) MICROBIT_BOOTLOADER_SETTINGS;
    if ( memcmp( pSettings, &settings, sizeof( nrf_dfu_settings_t)))
    {
      flash.erase_page( pSettings);
      flash.flash_burn( pSettings, (uint32_t *)&settings, settingsSizeInWords);
    }

    // save settings to flash settings backup page if different
    uint32_t *pBackup   = (uint32_t *) MICROBIT_MBR_PARAMS;
    if ( memcmp( pBackup, &settings, sizeof( nrf_dfu_settings_t)))
    {
      flash.erase_page( pBackup);
      flash.flash_burn( pBackup, (uint32_t *)&settings, settingsSizeInWords);
    }
}

/**
  * Write Event
  * Used the write data to the flash outside of the BLE ISR
  */
void MicroBitPartialFlashingService::partialFlashingEvent(MicroBitEvent e)
{
  MICROBIT_DEBUG_DMESG( "partialFlashingEvent");
  MicroBitFlash flash;

  switch(e.value){
    case FLASH_DATA:
    {
      MICROBIT_DEBUG_DMESG( "FLASH_DATA offset %x", (unsigned int) offset);
      /*
       * Set flashIncomplete flag if not already set to boot into BLE mode
       * upon a failed flash.
       */
       
       KeyValuePair* flashIncomplete = storage.get("flashIncomplete");
       if(flashIncomplete == NULL){

         uint8_t flashIncompleteVal = 0x01;
         storage.put("flashIncomplete", &flashIncompleteVal, sizeof(flashIncompleteVal));
            
         // Check if FS exists
         if(micropython_fs_end != 0x00) {
            for(uint32_t *page = (uint32_t *)micropython_fs_start; page < (uint32_t *)micropython_fs_end; page += (MICROBIT_CODEPAGESIZE / sizeof(uint32_t))) {
                // Check if page needs erasing
                for(uint32_t i = 0; i < 1024; i++) {
                    if(*(page + i) != 0xFFFFFFFF) {
                        DMESG( "Erase page at %x", page);
                        break; // If page has been erased we can skip the remaining bytes
                    }
                }
            }
         }

       }
       delete flashIncomplete;

      uint32_t *flashPointer   = (uint32_t *)(offset);

      // If the pointer is on a page boundary check if it needs erasing
      if(!((uint32_t)flashPointer % MICROBIT_CODEPAGESIZE)) {
          // Check words
          for(uint32_t i = 0; i < (MICROBIT_CODEPAGESIZE / sizeof(uint32_t)); i++) {
            if(*(flashPointer + i) != 0xFFFFFFFF) {
                flash.erase_page(flashPointer);
                break; // If page has been erased we can skip the remaining bytes
            }
          }

      }

      // Create a pointer to the data block
      uint32_t *blockPointer;
      blockPointer = block;

      // Write to flash
      flash.flash_burn(flashPointer, blockPointer, 16);

      // Update flash control buffer to send next packet
      uint8_t flashNotificationBuffer[] = {FLASH_DATA, 0xFF};
      notifyChrValue( mbbs_cIdxCTRL, (const uint8_t *)flashNotificationBuffer, sizeof(flashNotificationBuffer));
      break;
    }
    case END_OF_TRANSMISSION:
    {
      MICROBIT_DEBUG_DMESG( "END_OF_TRANSMISSION offset %x", (unsigned int) offset);
      // Write final packet
      uint32_t *blockPointer;
      uint32_t *flashPointer   = (uint32_t *) offset;

      blockPointer = block;
      flash.flash_burn(flashPointer, blockPointer, 16);

      // Set no validation
      noValidation();

      MICROBIT_DEBUG_DMESG( "rebooting");
      // Once the final packet has been written remove the BLE mode flag and reset
      // the micro:bit

      storage.remove("flashIncomplete");

      microbit_reset();
      break;
    }
    case MICROBIT_RESET:
    {
      MICROBIT_DEBUG_DMESG( "Calling restartInBLEMode");
      MicroBitBLEManager::manager->restartInBLEMode();
      break;
    }
  }

}

#endif
