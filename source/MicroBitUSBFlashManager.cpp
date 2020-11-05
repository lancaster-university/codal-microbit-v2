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

#define KL27_FLASH_ERASE_WORKAROUND 1

static const KeyValueTableEntry usbFlashPropertyLengthData[] = {
    {MICROBIT_USB_FLASH_FILENAME_CMD, 12},
    {MICROBIT_USB_FLASH_FILESIZE_CMD, 2},
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
        config.fileName = response;

        // Load the filesize
        response = transact(MICROBIT_USB_FLASH_FILESIZE_CMD);
        config.fileSize = response[1];

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
    ManagedBuffer fsize(2);
    ManagedBuffer fvisible(2);

    int dots = 0;
    bool invalidChar = false;

    // Ensure we know the device geometry
    getGeometry();

    // If the requested file is too long/short, we can't proceed.
    if (config.fileSize <= 0 || 1024 * config.fileSize >= geometry.blockSize*geometry.blockCount)
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
    fsize[1] = config.fileSize;

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

        // Esnure we don't cache invalid state.
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
 * @param length the number of bytes to read.
 * 
 * @return the data read, or a zero length buffer on failure.
 */
ManagedBuffer MicroBitUSBFlashManager::read(int address, int length)
{
    ManagedBuffer request(8);
    ManagedBuffer response;

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
 * Writes data to the specified location in the USB file staorage area.
 * 
 * @param data a buffer containing the data to write
 * @param address the location to write to
 * @param length the number of bytes to write
 * 
 * @return DEVICE_OK on success, or error code.
 */
int MicroBitUSBFlashManager::write(uint8_t *data, int address, int length)
{
    // Ensure address and length is 32 bit aligned (KL27 requirement)
    // Calculate word aligned address and length parameters taking into account any necessary padding
    int writeAddress = (address / 4) * 4;
    int padding1 = address - writeAddress;
    int padding2 = ((writeAddress + padding1 + length) % 4) == 0 ? 0 : 4 - ((writeAddress + padding1 + length) % 4);
    int writeLength = length + padding1 + padding2;

    ManagedBuffer request(writeLength + 8);
    ManagedBuffer response;

    // Ensure any padding bytes to 0xFF
    request.fill(0xFF);

    // Add header and user data
    uint32_t *p = (uint32_t *) &request[0];
    *p++ = htonl(((uint32_t) writeAddress) | (MICROBIT_USB_FLASH_WRITE_CMD << 24));
    *p++ = htonl(writeLength);
    memcpy(((uint8_t *)p) + padding1, &data[0], length);

    response = transact(request, 9);

    if (response.length() == 0)
        return DEVICE_I2C_ERROR;
    
    return DEVICE_OK;
}

/**
 * Writes data to the specified location in the USB file staorage area.
 * 
 * @param data a buffer containing the data to write
 * @param address the location to write to
 * 
 * @return DEVICE_OK on success, or error code.
 */
int MicroBitUSBFlashManager::write(ManagedBuffer data, int address)
{
    return write(&data[0], address, data.length());
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
 * @param start the logical address of the memory to start the erase operation.
 * @param end the logical address of the memory to end the erase operation.
 * 
 * @return DEVICE_OK on success, or error code.
 */
int MicroBitUSBFlashManager::erase(int address, int length)
{
    ManagedBuffer request(8);
    ManagedBuffer response;
    ManagedBuffer restoreBuffer1;
    ManagedBuffer restoreBuffer2;

    // Ensure we know the device geometry
    getGeometry();

    // Ensure we have a valid request
    if (length <= 0 || address < 0 || address + length > geometry.blockSize * geometry.blockCount)
        return DEVICE_INVALID_PARAMETER;

    // Calculcate block aligned start and end addresses for the erase operation, taking into account that he KL27 
    // uses INCLUSIVE end addresses.
    int eraseStart = (address / geometry.blockSize) * geometry.blockSize;
    int eraseEnd = (((address + length) / geometry.blockSize) * geometry.blockSize) - ((((address + length) % geometry.blockSize) == 0) ? geometry.blockSize : 0);

    // Determine if we need to undertake a "partial erase" of any blocks
    if (eraseStart != address || length != (eraseEnd - eraseStart) + geometry.blockSize)
    {
        // We need to take the slow path, and save/restore some data...
        // We also need to ensure our data is on a 32 bit boundary.
        int restoreLength1 = address - eraseStart;
        int restoreLength2 = (geometry.blockSize - ((address + length) % geometry.blockSize)) % geometry.blockSize;

        if (restoreLength1 > 0)
            restoreBuffer1 = read(eraseStart, restoreLength1);

        if (restoreLength2 > 0)
            restoreBuffer2 = read(address + length, restoreLength2);
    }

#ifdef KL27_FLASH_ERASE_WORKAROUND
    for (int page = eraseStart; page <= eraseEnd; page += geometry.blockSize)
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

#else
    // Workaround until block erasing supported.
    uint32_t *p = (uint32_t *) &request[0];
    *p++ = htonl(eraseStart | (MICROBIT_USB_FLASH_ERASE_CMD << 24));
    *p++ = htonl(eraseEnd);

    DMESG("ERASING: [start: %d] [end: %d]", eraseStart, eraseEnd);
    for (int i=0; i<request.length();i++)
        DMESG("%x ", request[i]);

    response = transact(request, 1);

    if (response.length() == 0)
    {
        DMESG("ERROR ERASING");
        return DEVICE_I2C_ERROR;
    }
#endif

    // Restore any saved data if necessary
    if (restoreBuffer1.length() > 0)
        this->write(restoreBuffer1, eraseStart);
    
    if (restoreBuffer2.length() > 0)
        this->write(restoreBuffer2, address + length);

    return DEVICE_OK;
}

/**
 * Performs a flash storage transaction with the interface chip.
 * @param packet The data to write to the interface chip as a request operation.
 * @param responseLength The length of the expected reponse packet.
 * @return a buffer containing the response to the request, or a zero length buffer on failure.
 */
ManagedBuffer MicroBitUSBFlashManager::transact(ManagedBuffer request, int responseLength)
{
    int attempts = 0;

    power.nop();
    if (i2cBus.write(MICROBIT_USB_FLASH_I2C_ADDRESS, &request[0], request.length(), false) != DEVICE_OK)
        return ManagedBuffer();
 
    power.awaitingPacket(true);

    while(attempts < MICROBIT_USB_FLASH_MAX_RETRIES)
    {   
        fiber_sleep(5);

        if(io.irq1.isActive())
        {
            ManagedBuffer b(responseLength);

            power.nop();
            int r = i2cBus.read(MICROBIT_USB_FLASH_I2C_ADDRESS, &b[0], responseLength, false);
            
            if (r == MICROBIT_OK)
            {               
                power.awaitingPacket(false);
                return b;
            }
        }
                  
        attempts++;
    }

    DMESG("USB_FLASH: *** IRQ TIMEOUT ***");
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
