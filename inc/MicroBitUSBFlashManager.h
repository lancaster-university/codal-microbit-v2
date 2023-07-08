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

#ifndef MICROBIT_USB_FLASH_MANAGER_H
#define MICROBIT_USB_FLASH_MANAGER_H

#include "MicroBitConfig.h"
#include "MicroBitCompat.h"
#include "MicroBitIO.h"
#include "codal-core/inc/core/CodalComponent.h"
#include "codal-core/inc/driver-models/I2C.h"
#include "codal-core/inc/driver-models/Pin.h"
#include "MicroBitPowerManager.h"
#include "NVMController.h"


// Constants for USB Interface Flash Management Protocol

//
// I2C Address of the USB Interface Chip (Flash interface)
//
#define MICROBIT_USB_FLASH_I2C_ADDRESS	            (0x72 << 1)

//
// General constants 
//
#ifndef MICROBIT_USB_FLASH_MAX_TX_RETRIES
#define MICROBIT_USB_FLASH_MAX_TX_RETRIES           20
#endif

#ifndef MICROBIT_USB_FLASH_MAX_RX_RETRIES
#define MICROBIT_USB_FLASH_MAX_RX_RETRIES           20
#endif

#ifndef MICROBIT_USB_FLASH_MAX_FLASH_STORAGE
#define MICROBIT_USB_FLASH_MAX_FLASH_STORAGE        0x1F000
#endif


//
// Command codes for the USB Interface Chip
//
#define MICROBIT_USB_FLASH_FILENAME_CMD             0x01            // Defines the filename exposed via MICROBIT drive.
#define MICROBIT_USB_FLASH_FILESIZE_CMD             0x02            // Defines the size of the file exposed via the MICROBIT drive (KB).
#define MICROBIT_USB_FLASH_VISIBILITY_CMD           0x03            // Define if the user file should be visible on the MICROBIT drive.
#define MICROBIT_USB_FLASH_WRITE_CONFIG_CMD         0x04            // Write current config of above commands to persistent FLASH.
#define MICROBIT_USB_FLASH_ERASE_CONFIG_CMD         0x05            // Erase the config above stored in peristent FLASH.
#define MICROBIT_USB_FLASH_DISK_SIZE_CMD            0x06            // Determine the amount of availble FLASH space for application use, in KB.
#define MICROBIT_USB_FLASH_SECTOR_SIZE_CMD          0x07            // Determine the size of a physical FLASH sector.
#define MICROBIT_USB_FLASH_REMOUNT_CMD              0x08            // Remount the MICORBIT drive, taking into account any changes to config.
#define MICROBIT_USB_FLASH_STATUS_CMD               0x09            // Determine if USB interface chip is BUSY (unused).
#define MICROBIT_USB_FLASH_READ_CMD                 0x0A            // Perform a READ operation from applicaiton FLASH memory.
#define MICROBIT_USB_FLASH_WRITE_CMD                0x0B            // Perform a WRITE operation from applicaiton FLASH memory.
#define MICROBIT_USB_FLASH_ERASE_CMD                0x0C            // Perform a ERASE operation from applicaiton FLASH memory.

typedef struct
{
    ManagedString       fileName;                                   // MUST be in 8.3 format
    int                 fileSize;                                   // File Size in KB (Odd - change to bytes?)
    bool                visible;                                    // Whether or not this file should be visible on the MICROBIT drive
} MicroBitUSBFlashConfig;

typedef struct
{
    uint16_t            blockSize;                                  // The size of a physical block on the disk
    uint8_t             blockCount;                                 // The number of available blocks on the disk
} MicroBitUSBFlashGeometry;


//
// Component Status flags
//
#define MICROBIT_USB_FLASH_AWAITING_RESPONSE        0x01
#define MICROBIT_USB_FLASH_GEOMETRY_LOADED          0x02
#define MICROBIT_USB_FLASH_CONFIG_LOADED            0x04
#define MICROBIT_USB_FLASH_SINGLE_PAGE_ERASE_ONLY   0x08
#define MICROBIT_USB_FLASH_USE_NULL_TRANSACTION     0x10
#define MICROBIT_USB_FLASH_BUSY_FLAG_SUPPORTED      0x20
#define MICROBIT_USB_FLASH_100MS_AFTER_ERASE        0x40


/**
 * Class definition for MicroBitUSBFlashManager.
 */
class MicroBitUSBFlashManager : public CodalComponent, public NVMController
{
    private:
        MicroBitI2C                 &i2cBus;                            // The I2C bus to use to communicate with USB interface chip.
        MicroBitIO                  &io;                                // Pins used by this device
        MicroBitPowerManager        &power;                             // Reference to power manager instance
        MicroBitUSBFlashConfig      config;                             // Current configuration of the USB File interface
        MicroBitUSBFlashGeometry    geometry;                           // Current geomtry of the USB File interface
        int                         maxWriteLength;                     // The maximum number of bytes that can be written in a single transaction.

    public:
        /**
         * Constructor.
         * Create a software abstraction of a USB Flash manager.
         *
         * @param i2c the I2C bus to use to communicate with the micro:bit USB interface chip
         * @param ioPins the IO pins in use on this device.
         * @param id the unique EventModel id of this component. Defaults to: MICROBIT_ID_USB_FLASH_MANAGER
         *
         */
        MicroBitUSBFlashManager(MicroBitI2C &i2c, MicroBitIO &ioPins, MicroBitPowerManager &powerManager, uint16_t id = MICROBIT_ID_USB_FLASH_MANAGER);

        /**
         * Determines the filename, filesize and visibility of the USB file presented on the MICROBIT drive.
         * note: This may query the USB interface chip via I2C, and wait for completion.
         * 
         * @return a configuration structure containing the current USB File interface confuguration.
         */
        MicroBitUSBFlashConfig getConfiguration();

        /**
         * Updates the filename, filesize and visibility of the USB file presented on the MICROBIT drive.
         * note: This will update the USB interface chip via I2C, and wait for completion.
         * 
         * @param config the new configuration to write
         * @param persist true if the configuraiton should be permananet across device reset, false otherwise.
         * 
         * @return DEVICE_OK on success, or error code.
         */
        int setConfiguration(MicroBitUSBFlashConfig config, bool persist = false);

        /**
         * Determines the physical blocksize and number of blocks available in the USB file storage.
         * note: This may query the USB interface chip via I2C, and wait for completion.
         * 
         * @return a configuration structure containing the geeometry of the USB file storage area.
         */
        MicroBitUSBFlashGeometry getGeometry();

        /**
         * Remounts the USB MICROBIT drive, if it is connected.
         * 
         * @return DEVICE_OK on success, or error code.
         */
        int remount();

        /**
         * Reset the KL27 Flash Storage configuration to its default settings.
         * 
         * @return DEVICE_OK on success, or error code.
         */
        int eraseConfig();

        /**
         * Reads data from the specified location in the USB file storage area.
         * 
         * @param address the logical address of the memory to read.
         * @param length the number of 32-bit words to read.
         * 
         * @return the data read, or a zero length buffer on failure.
         */
        ManagedBuffer read(uint32_t address, uint32_t length);

        /**
         * Reads a block of memory from non-volatile memory into RAM
         * 
         * @param dest The address in RAM in which to store the result of the read operation
         * @param address The logical address in non-voltile memory to read from
         * @param length The number 32-bit words to read.
         */ 
        virtual int read(uint32_t* dest, uint32_t address, uint32_t length) override;

        /**
         * Writes data to the specified location in the USB file staorage area.
         * 
         * @param data a buffer containing the data to write
         * @param address the location to write to
         * @param length the number of 32-bit words to write
         * 
         * @return DEVICE_OK on success, or error code.
         */
        int write(uint32_t address, uint32_t *data, uint32_t length) override;

        /**
         * Writes data to the specified location in the USB file staorage area.
         * 
         * @param data a buffer containing the data to write. Length MUST be 32-bit aligned.
         * @param address the location to write to
         * 
         * @return DEVICE_OK on success, or error code.
         */
        int write(ManagedBuffer data, uint32_t address);

        /**
         * Flash Erase one or more physical blocks in the USB file staorage area.
         * n.b. this method will perform an ERASE operation on all physical
         * memory blocks that are are part of the specified region. 
         *
         * If the specified region is not aligned to physical block boundaries as
         * defined by the disks geometry, areas in an overlapping block outside the given 
         * range will be automatically read, erased and rewritten. 
         * 
         * @param address the logical address of the memory to start the erase operation.
         * @param length the number of 32-bit words to erase.
         * 
         * @return DEVICE_OK on success, or error code.
         */
        int erase(uint32_t address, uint32_t length);

        /**
         * Erases a given page in non-volatile memory.
         * 
         * @param page The address of the page to erase (logical address of the start of the page).
         */
        virtual int erase(uint32_t page) override;


        /**
         * Determines the logical address of the start of non-volatile memory region
         * 
         * @return The logical address of the first valid logical address in the region of non-volatile memory
         */
        virtual uint32_t getFlashStart() override;

        /**
         * Determines the logical address of the end of the non-volatile memory region
         * 
         * @return The logical address of the first invalid logical address beyond
         * the non-volatile memory.
         */
        virtual uint32_t getFlashEnd() override;

        /**
         * Determines the size of a non-volatile memory page. A page is defined as
         * the block of memory the can be efficiently erased without impacted on
         * other regions of memory.
         * 
         * @return The size of a single page in bytes.
         */
        virtual uint32_t getPageSize() override;

        /**
         * Determines the amount of available storage.
         * 
         * @return the amount of available storage, in bytes.
         */
        virtual uint32_t getFlashSize() override;

        /**
         * Destructor.
         */
        ~MicroBitUSBFlashManager();

        private:

        /**
         * Performs a flash storage transaction with the interface chip.
         * @param packet The data to write to the interface chip as a request operation.
         * @param responseLength The length of the expected reponse packet.
         * @return a buffer containing the response to the request, or a zero length buffer on failure.
         */
        ManagedBuffer transact(ManagedBuffer request, int responseLength);
        ManagedBuffer _transact(ManagedBuffer request, int responseLength);

        /**
         * Performs a flash storage transaction with the interface chip.
         * @param command Identifier of a command to issue (one byte write operation).
         * @return a buffer containing the response to the request, or a zero length buffer on failure.
         */
        ManagedBuffer transact(int command);

        /**
         * Determines if the given char is valid for an 8.3 filename.
         */
        bool isValidChar(char c);

};
#endif
