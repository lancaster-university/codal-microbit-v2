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

#include "MicroBitUSBFlashManager.h"

static const KeyValueTableEntry usbFlashPropertyLengthData[] = {
    {MICROBIT_USB_FLASH_FILENAME_CMD, 12},
    {MICROBIT_USB_FLASH_FILESIZE_CMD, 5},
    {MICROBIT_USB_FLASH_VISIBILITY_CMD, 2},
    {MICROBIT_USB_FLASH_WRITE_CONFIG_CMD, 1},
    {MICROBIT_USB_FLASH_ERASE_CONFIG_CMD, 1},
    {MICROBIT_USB_FLASH_DISK_SIZE_CMD, 2},
    {MICROBIT_USB_FLASH_SECTOR_SIZE_CMD, 3},
    {MICROBIT_USB_FLASH_REMOUNT_CMD, 1}
};
CREATE_KEY_VALUE_TABLE(usbFlashPropertyLength, usbFlashPropertyLengthData);

/**
 * Constructor.
 * Create a software abstraction of a USB Flash manager.
 *
 * @param i2c the I2C bus to use to communicate with the micro:bit USB interface chip
 * @param ioPins the IO pins in use on this device.
 * @param id the unique EventModel id of this component. Defaults to: MICROBIT_ID_USB_FLASH_MANAGER
 *
 */
MicroBitUSBFlashManager::MicroBitUSBFlashManager(MicroBitI2C &i2c, MicroBitIO &ioPins, MicroBitPowerManager &powerManager, uint16_t id) : i2cBus(i2c), io(ioPins), power(powerManager)
{
    this->id = id;
    this->maxWriteLength = 64;

    // Be pessimistic about the interface chip in use, until we obtain version information.
    status = (MICROBIT_USB_FLASH_SINGLE_PAGE_ERASE_ONLY | MICROBIT_USB_FLASH_USE_NULL_TRANSACTION);
}

/**
 * Determines the filename, filesize and visibility of the USB file presented on the MICROBIT drive.
 * note: This may query the USB interface chip via I2C, and wait for completion.
 * 
 * @return a configuration structure containing the current USB File interface confuguration.
 */
MicroBitUSBFlashConfig MicroBitUSBFlashManager::getConfiguration()
{
    // We cache the config, as it can only be changed by the nrf52...
    if (!(status & MICROBIT_USB_FLASH_CONFIG_LOADED))
    {
        ManagedBuffer response;

        // Load the configured filename
        response = transact(MICROBIT_USB_FLASH_FILENAME_CMD);
        if (response.length() > 5)
        {
            ManagedBuffer n = response.slice(1, response.length()-1);
            n[n.length()-4] = '.';
            config.fileName = n;
        }

        // Load the filesize
        response = transact(MICROBIT_USB_FLASH_FILESIZE_CMD);
        uint32_t s;
        memcpy(&s, &response[1], 4);
        config.fileSize = htonl(s);

        // Load the visibility status
        response = transact(MICROBIT_USB_FLASH_VISIBILITY_CMD);
        config.visible = response[1] == 0 ? 0 : 1;

        status |= MICROBIT_USB_FLASH_CONFIG_LOADED;
    }

    return config;    
}

/**
 * Determines if the given char is valid for an 8.3 filename.
 */
bool MicroBitUSBFlashManager::isValidChar(char c)
{
    return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c>= 'a' && c<= 'z') || c == '-' || c== '_');
}

/**
 * Updates the filename, filesize and visibility of the USB file presented on the MICROBIT drive.
 * note: This will update the USB interface chip via I2C, and wait for completion.
 * 
 * @param config the new configuration to write
 * @param persist true if the configuraiton should be permananet across device reset, false otherwise.
 * 
 * @return DEVICE_OK on success, or error code.
 */
int MicroBitUSBFlashManager::setConfiguration(MicroBitUSBFlashConfig config, bool persist)
{
    ManagedBuffer fname(12);
    ManagedBuffer fsize(5);
    ManagedBuffer fvisible(2);

    int dots = 0;
    bool invalidChar = false;

    // Ensure we know the device geometry
    getGeometry();

    // If the requested file is too long/short, we can't proceed.
    if (config.fileSize <= 0 || config.fileSize > geometry.blockSize*geometry.blockCount)
        return MICROBIT_INVALID_PARAMETER;

    // Validate filename as fixed length 8.3 format, as required by USB interface chip.
    for (int i=0; i<config.fileName.length(); i++)
    {
        if (config.fileName.charAt(i) =='.')
            dots++;
        else if (!isValidChar(config.fileName.charAt(i)))
            invalidChar = true;        
    }

    // Blimey 8.3 is complex. :)
    if (config.fileName.length() <5 || config.fileName.length() > 12 || dots == 0 || dots > 1 || invalidChar || config.fileName.charAt(config.fileName.length()-4) != '.')
    {
        DMESG("INVALID FILENAME");
        return MICROBIT_INVALID_PARAMETER;
    }

    // We have a valid filename. Encode it as needed.
    // First, ensure unused characters are ASCII spaces
    for (int i=0; i<fname.length(); i++)
        fname[i] = ' ';

    // Now copy in the filename and extension (right justified extension)
    fname[0] = MICROBIT_USB_FLASH_FILENAME_CMD;
    memcpy(&fname[1], config.fileName.toCharArray(), config.fileName.length() - 4);
    memcpy(&fname[9], config.fileName.toCharArray() + config.fileName.length() - 3, 3);

    // Encode file size command.
    fsize[0] = MICROBIT_USB_FLASH_FILESIZE_CMD;
    uint32_t s = htonl(config.fileSize);
    memcpy(&fsize[1], &s, 4);

    // Encode visibility command
    fvisible[0] = MICROBIT_USB_FLASH_VISIBILITY_CMD;
    fvisible[1] = config.visible ? 1 : 0;

    // Write out each of the parameters in turn.
    transact(fname, 12);
    transact(fsize, 2);
    transact(fvisible, 2);

    if (persist)
        transact(MICROBIT_USB_FLASH_WRITE_CONFIG_CMD);

    // Cache for later.
    this->config = config;

    return DEVICE_OK;
}

/**
 * Determines the physical blocksize and number of blocks available in the USB file storage.
 * note: This may query the USB interface chip via I2C, and wait for completion.
 * 
 * @return a configuration structure containing the geeometry of the USB file storage area.
 */
MicroBitUSBFlashGeometry MicroBitUSBFlashManager::getGeometry()
{
    bool valid = true;

    // We cache the config, as it can only be changed by the nrf52...
    if (!(status & MICROBIT_USB_FLASH_GEOMETRY_LOADED))
    {
        ManagedBuffer response;

        // Load the block size data
        response = transact(MICROBIT_USB_FLASH_SECTOR_SIZE_CMD);
        if (response.length() > 0)
            geometry.blockSize = htons(*(uint16_t *) &response[1]);
        else
            valid = false;

        // Load the number of blocks
        response = transact(MICROBIT_USB_FLASH_DISK_SIZE_CMD);
        if (response.length() > 0)
            geometry.blockCount = *(uint8_t *) &response[1];
        else
            valid = false;

        // Optimize behaviour for the interface chip version in use.
        if (valid)
        {
            // Apply FLASH storage limiting policy to align V2.0 and V2.2 device capabilities.
            if (geometry.blockCount * geometry.blockSize > MICROBIT_USB_FLASH_MAX_FLASH_STORAGE)
                geometry.blockCount = MICROBIT_USB_FLASH_MAX_FLASH_STORAGE / geometry.blockSize;

            MicroBitVersion v = power.getVersion();
            switch(v.i2c)
            {
                case 1:
                    // Apply workarounds for V2.00 KL27 release.
                    maxWriteLength = 64;
                    status |= (MICROBIT_USB_FLASH_SINGLE_PAGE_ERASE_ONLY | MICROBIT_USB_FLASH_USE_NULL_TRANSACTION);
                    break;

                case 2:
                    // Assume fixes released from V2.2 onwards.

                    // TODO: Determine if writelength can be increased in this revision.
                    maxWriteLength = 64;

                    // TODO: Determine if multiple page erase is supported in this revision.
                    status |= MICROBIT_USB_FLASH_SINGLE_PAGE_ERASE_ONLY;

                    // TODO: Remove this in favour of MICROBIT_USB_FLASH_BUSY_FLAG_SUPPORTED once merged.
                    status |= MICROBIT_USB_FLASH_USE_NULL_TRANSACTION;
                    break;

                default:
                    maxWriteLength = 64;
                    status &= ~MICROBIT_USB_FLASH_USE_NULL_TRANSACTION;
                    status |= MICROBIT_USB_FLASH_BUSY_FLAG_SUPPORTED;
                    //status &= ~MICROBIT_USB_FLASH_SINGLE_PAGE_ERASE_ONLY
            }
        }

        // Ensure we don't cache invalid state.
        if (valid)
            status |= MICROBIT_USB_FLASH_GEOMETRY_LOADED;
    }

    return geometry;
}

/**
 * Remounts the USB MICROBIT drive, if it is connected.
 * 
 * @return DEVICE_OK on success, or error code.
 */
int MicroBitUSBFlashManager::remount()
{
    ManagedBuffer cmd(1);

    cmd[0] = MICROBIT_USB_FLASH_REMOUNT_CMD;
    transact(cmd, 1);

    return DEVICE_OK;
}

/**
 * Reset the KL27 Flash Storage configuration to its default settings.
 * 
 * @return DEVICE_OK on success, or error code.
 */
int MicroBitUSBFlashManager::eraseConfig()
{
    ManagedBuffer cmd(1);

    cmd[0] = MICROBIT_USB_FLASH_ERASE_CONFIG_CMD;
    transact(cmd, 1);

    return DEVICE_OK;
}

/**
 * Reads data from the specified location in the USB file staorage area.
 * 
 * @param address the logical address of the memory to read.
 * @param length the number of 32-bit words to read.
 * 
 * @return the data read, or a zero length buffer on failure.
 */
ManagedBuffer 
MicroBitUSBFlashManager::read(uint32_t address, uint32_t length)
{
    ManagedBuffer request(8);
    ManagedBuffer response;

    // Convert length parameter from 32-bit count to a byte count.
    length = length * sizeof(uint32_t);

    uint32_t *p = (uint32_t *) &request[0];
    *p++ = htonl((uint32_t) address | (MICROBIT_USB_FLASH_READ_CMD << 24));
    *p++ = htonl(length);

    response = transact(request, length + 8);

    // If we have a valid repsonse, strip off the KL27 I2C Header
    if (response.length() > 8)
    {
        response.shift(8);
        response.truncate(response.length() - 8);
    }
    else
    {
        response = ManagedBuffer();
    }
    
    return response;
}

/**
 * Reads a block of memory from non-volatile memory into RAM
 * 
 * @param dest The address in RAM in which to store the result of the read operation
 * @param address The logical address in non-voltile memory to read from
 * @param length The number 32-bit words to read.
 */ 
int 
MicroBitUSBFlashManager::read(uint32_t* dest, uint32_t address, uint32_t length)
{
    ManagedBuffer b = read(address, length);

    if (b.length() == 0)
        return DEVICE_NO_DATA;
    
    memcpy(dest, &b[0], length * sizeof(uint32_t));
    return DEVICE_OK;
}

/**
 * Writes data to the specified location in the USB file staorage area.
 * 
 * @param data a buffer containing the data to write
 * @param address the location to write to. Must be 32 bit aligned.
 * @param length the number of 32 bit words to write
 * 
 * @return DEVICE_OK on success, or error code.
 */
int MicroBitUSBFlashManager::write(uint32_t address, uint32_t *data, uint32_t length)
{
    // Convert length parameter from 32-bit count to a byte count.
    length = length * sizeof(uint32_t);

    ManagedBuffer response;

    ManagedBuffer request(min(maxWriteLength, length) + 8);
    request.fill(0xFF);

    uint32_t p = address;
    uint32_t end = p + length;
    uint32_t bytesWritten = 0;
    uint32_t segmentLength;

    while (p < end)
    {
        segmentLength = min(maxWriteLength, length-bytesWritten);
        
        uint32_t *ptr = (uint32_t *) &request[0];
        *ptr++ = htonl( p | (MICROBIT_USB_FLASH_WRITE_CMD << 24));
        *ptr++ = htonl(segmentLength);
        memcpy(ptr, data + (bytesWritten/4), segmentLength);

        request.truncate(segmentLength + 8);
        response = transact(request, 9);

        if (response.length() == 0)
            return DEVICE_I2C_ERROR;

        bytesWritten += segmentLength;
        p+= segmentLength;
    }

    return DEVICE_OK;
}

/**
 * Writes data to the specified location in the USB file staorage area.
 * 
 * @param data a buffer containing the data to write. Length MUST be 32-bit aligned.
 * @param address the location to write to.
 * 
 * @return DEVICE_OK on success, or error code.
 */
int MicroBitUSBFlashManager::write(ManagedBuffer data, uint32_t address)
{
    return write(address, (uint32_t *)&data[0], data.length()/sizeof(uint32_t));
}

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
int MicroBitUSBFlashManager::erase(uint32_t address, uint32_t length)
{
    ManagedBuffer request(8);
    ManagedBuffer response;
    ManagedBuffer restoreBuffer1;
    ManagedBuffer restoreBuffer2;

    // Ensure we know the device geometry
    getGeometry();

    // Convert length parameter from 32-bit count to a byte count.
    length = length * sizeof(uint32_t);

    // Ensure we have a valid request
    if ((address + length > geometry.blockSize * geometry.blockCount) || (length % sizeof(uint32_t) != 0)) 
        return DEVICE_INVALID_PARAMETER;

    // Calculcate block aligned start and end addresses for the erase operation, taking into account that he KL27 
    // uses INCLUSIVE end addresses.
    uint32_t eraseStart = (address / geometry.blockSize) * geometry.blockSize;
    uint32_t eraseEnd = (((address + length) / geometry.blockSize) * geometry.blockSize) - ((((address + length) % geometry.blockSize) == 0) ? geometry.blockSize : 0);

    // Determine if we need to undertake a "partial erase" of any blocks
    if (eraseStart != address || length != (eraseEnd - eraseStart) + geometry.blockSize)
    {
        // We need to take the slow path, and save/restore some data...
        // We also need to ensure our data is on a 32 bit boundary.
        int restoreLength1 = address - eraseStart;
        int restoreLength2 = (geometry.blockSize - ((address + length) % geometry.blockSize)) % geometry.blockSize;

        if (restoreLength1 > 0)
            restoreBuffer1 = read(eraseStart, restoreLength1/sizeof(uint32_t));

        if (restoreLength2 > 0)
            restoreBuffer2 = read(address + length, restoreLength2/sizeof(uint32_t));
    }

    if (status & MICROBIT_USB_FLASH_SINGLE_PAGE_ERASE_ONLY)
    {
        for (uint32_t page = eraseStart; page <= eraseEnd; page += geometry.blockSize)
        {
            uint32_t *p = (uint32_t *) &request[0];
            *p++ = htonl(page | (MICROBIT_USB_FLASH_ERASE_CMD << 24));
            *p++ = htonl(page);

            response = transact(request, 1);
            if (response.length() == 0)
            {
                DMESG("ERROR ERASING");
                return DEVICE_I2C_ERROR;
            }
        }
    }
    else
    {
        uint32_t *p = (uint32_t *) &request[0];
        *p++ = htonl(eraseStart | (MICROBIT_USB_FLASH_ERASE_CMD << 24));
        *p++ = htonl(eraseEnd);

        response = transact(request, 1);
        if (response.length() == 0)
        {
            DMESG("ERROR ERASING");
            return DEVICE_I2C_ERROR;
        }
    }

    // Restore any saved data if necessary
    if (restoreBuffer1.length() > 0)
        this->write(restoreBuffer1, eraseStart);
    
    if (restoreBuffer2.length() > 0)
        this->write(restoreBuffer2, (address + length));

    return DEVICE_OK;
}

/**
 * Erases a given page in non-volatile memory.
 * 
 * @param page The address of the page to erase (logical address of the start of the page).
 */
int 
MicroBitUSBFlashManager::erase(uint32_t page)
{
    getGeometry();
    return erase(page, geometry.blockSize/4);
}

/**
 * Determines the logical address of the start of non-volatile memory region
 * 
 * @return The logical address of the first valid logical address in the region of non-volatile memory
 */
uint32_t
MicroBitUSBFlashManager::getFlashStart()
{
    // We index from logival address zero.
    return 0;
    
}

/**
 * Determines the logical address of the end of the non-volatile memory region
 * 
 * @return The logical address of the first invalid logical address beyond
 * the non-volatile memory.
 */
uint32_t
MicroBitUSBFlashManager::getFlashEnd()
{
    getGeometry();
    return geometry.blockSize * geometry.blockCount;
}

/**
 * Determines the size of a non-volatile memory page. A page is defined as
 * the block of memory the can be efficiently erased without impacted on
 * other regions of memory.
 * 
 * @return The size of a single page in bytes.
 */
uint32_t
MicroBitUSBFlashManager::getPageSize()
{
    getGeometry();
    return geometry.blockSize;
}

/**
 * Determines the amount of available storage.
 * 
 * @return the amount of available storage, in bytes.
 */
uint32_t
MicroBitUSBFlashManager::getFlashSize()
{
    return getFlashEnd();
}

/**
 * Performs a flash storage transaction with the interface chip.
 * @param packet The data to write to the interface chip as a request operation.
 * @param responseLength The length of the expected reponse packet.
 * @return a buffer containing the response to the request, or a zero length buffer on failure.
 */
ManagedBuffer MicroBitUSBFlashManager::transact(ManagedBuffer request, int responseLength)
{
    power.nop();

    if (status & MICROBIT_USB_FLASH_USE_NULL_TRANSACTION)
    {
        ManagedBuffer nop_request(1);
        nop_request[0] = request[0] == MICROBIT_USB_FLASH_VISIBILITY_CMD ? MICROBIT_USB_FLASH_DISK_SIZE_CMD : MICROBIT_USB_FLASH_VISIBILITY_CMD;
        _transact(nop_request, usbFlashPropertyLength.get(MICROBIT_USB_FLASH_VISIBILITY_CMD));
    }

    return _transact(request, responseLength);
}

/**
 * Performs a flash storage transaction with the interface chip.
 * @param packet The data to write to the interface chip as a request operation.
 * @param responseLength The length of the expected reponse packet.
 * @return a buffer containing the response to the request, or a zero length buffer on failure.
 */
ManagedBuffer MicroBitUSBFlashManager::_transact(ManagedBuffer request, int responseLength)
{
    int tx_attempts = 0;
    int rx_attempts = 0;

    ManagedBuffer b(max(responseLength, 3));

    while(tx_attempts < MICROBIT_USB_FLASH_MAX_TX_RETRIES)
    {
        rx_attempts = 0;
        tx_attempts++;

        power.awaitingPacket(true);

        if (i2cBus.write(MICROBIT_USB_FLASH_I2C_ADDRESS, &request[0], request.length(), false) != DEVICE_OK)
        {
            DMESG("TRANSACT: [I2C WRITE ERROR]");
            fiber_sleep(1);
            continue;
        }

        target_wait_us(200);

        while(rx_attempts < MICROBIT_USB_FLASH_MAX_RX_RETRIES)
        {
            rx_attempts++;

            if(io.irq1.isActive())
            {
                b.fill(0);
                int r = i2cBus.read(MICROBIT_USB_FLASH_I2C_ADDRESS, &b[0], b.length(), false);

                if (r == MICROBIT_OK)
                {
                    if (b[0] == request[0])
                    {
                        // We have a valid response. Consume it, and we're done.
                        power.awaitingPacket(false);
                        b.truncate(responseLength);
                        return b;
                    }
                    else
                    {
                        // We have a negative response. If it's not a FAIL case, treat this as "NOT READY"
                        // reset RX timeout, as the peripheral is active on our transaction.
                        // some revisions report busy status explicitly, others do not and we must infer...
                        bool busy = (status & MICROBIT_USB_FLASH_BUSY_FLAG_SUPPORTED) ? b[0] == 0x20 && b[1] == 0x39 : b[0] == 0x00 || (b[0] == 0x20 && (b[1] == request[0] || b[1] == 0x00));

                        if (busy)
                        {
                            //DMESG("TRANSACT: [BUSY: LEN: %d]", responseLength);
                            //for (int i=0; i<responseLength; i++)
                            //    DMESGN("%d ", b[i]);
                            //DMESGN("\n");
                            rx_attempts = 0;
                        }
                        else
                        {
                            DMESG("TRANSACT: [UNEXPECTED RESPONSE: LEN: %d]", responseLength);
                            for (int i=0; i<responseLength; i++)
                                DMESGN("%d ", b[i]);
                            DMESGN("\n");
                            break;
                        }
                    }
                }
                else
                {
                    DMESG("TRANSACT: [I2C READ ERROR: %d]",r);
                    break;
                }
            }

            fiber_sleep(1);
        }
    }

    DMESG("USB_FLASH: Transaction Failed.");
    power.awaitingPacket(false);
    return ManagedBuffer();
}

/**
 * Performs a flash storage transaction with the interface chip.
 * @param command Identifier of a command to issue (one byte write operation).
 * @param responseLength The length of the expected reponse packet.
 * @return a buffer containing the response to the request, or a zero length buffer on failure.
 */
ManagedBuffer MicroBitUSBFlashManager::transact(int command)
{
    ManagedBuffer request(1);

    request[0] = command;
    return transact(request, usbFlashPropertyLength.get(command));
}

/**
 * Destructor.
 */
MicroBitUSBFlashManager::~MicroBitUSBFlashManager()
{

}
