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
    {MICROBIT_USB_FLASH_FILESIZE_CMD, 3},
    {MICROBIT_USB_FLASH_VISIBILITY_CMD, 2},
    {MICROBIT_USB_FLASH_WRITE_CONFIG_CMD, 1},
    {MICROBIT_USB_FLASH_ERASE_CONFIG_CMD, 1},
    {MICROBIT_USB_FLASH_DISK_SIZE_CMD, 2},
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
MicroBitUSBFlashManager::MicroBitUSBFlashManager(MicroBitI2C &i2c, MicroBitIO &ioPins, uint16_t id) : i2cBus(i2c), io(ioPins)
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
        ManagedString s = response;
        config.fileName = s.substring(0, 8) + "." + s.substring(8, 3);

        // Load the filesize
        response = transact(MICROBIT_USB_FLASH_FILESIZE_CMD);
        config.fileSize = *(uint32_t *) &response[0];

        // Load the visibility status
        response = transact(MICROBIT_USB_FLASH_VISIBILITY_CMD);
        config.visible = response[0] != 0;

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
        return MICROBIT_INVALID_PARAMETER;

    // We have a valid filename. Encode it as needed.
    // First, ensure unused characters are ASCII spaces
    for (int i=0; i<fname.length(); i++)
        fname[i] = ' ';

    // Now copy in the filename and extension (right justified extension)
    fname[0] = MICROBIT_USB_FLASH_FILENAME_CMD;
    memcpy(&fname[1], config.fileName.toCharArray(), config.fileName.length());
    fname[config.fileName.length()-3] = ' ';

    // Encode file size command.
    fsize[0] = MICROBIT_USB_FLASH_FILESIZE_CMD;
    fsize[1] = config.fileSize;

    // Encode visibility command
    fvisible[0] = MICROBIT_USB_FLASH_VISIBILITY_CMD;
    fvisible[1] = config.visible ? 1 : 0;

    // Write out each of the parameters in turn.
    transact(fname, 1);
    transact(fsize, 1);
    transact(fvisible, 1);

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
   // We cache the config, as it can only be changed by the nrf52...
    if (!(status & MICROBIT_USB_FLASH_GEOMETRY_LOADED))
    {
        ManagedBuffer response;

        // Load the block size data
        response = transact(MICROBIT_USB_FLASH_SECTOR_SIZE_CMD);
        geometry.blockSize = *(uint16_t *) &response[2];

        // Load the number of blocks
        response = transact(MICROBIT_USB_FLASH_DISK_SIZE_CMD);
        geometry.blockCount = *(uint8_t *) &response[2];

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

    uint32_t *p = (uint32_t *) &request[0];
    *p++ = ((uint32_t) address) | (MICROBIT_USB_FLASH_READ_CMD << 24);
    *p++ = (uint32_t) (length);


    return transact(request, length+8);
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
    ManagedBuffer request(8+length);
    ManagedBuffer response;

    uint32_t *p = (uint32_t *) &request[0];
    *p++ = ((uint32_t) address) | (MICROBIT_USB_FLASH_WRITE_CMD << 24);
    *p++ = (uint32_t) (length);
    memcpy(p, &data[0], length);

    response = transact(request, 1);

    if (response.length() == 0)
        return DEVICE_I2C_ERROR;
    else
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

    uint32_t *p = (uint32_t *) &request[0];
    *p++ = ((uint32_t) address) | (MICROBIT_USB_FLASH_ERASE_CMD << 24);
    *p++ = (uint32_t) (address+length);

    response = transact(request, 1);

    if (response.length() == 0)
        return DEVICE_I2C_ERROR;
    else
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

    if (i2cBus.write(MICROBIT_USB_FLASH_I2C_ADDRESS, &request[0], request.length(), false) != DEVICE_OK)
        return ManagedBuffer();

    while(attempts < MICROBIT_USB_FLASH_MAX_RETRIES)
    {
        fiber_sleep(5);
        if(io.irq1.isActive())
        {
            ManagedBuffer b(responseLength);
            if (i2cBus.read(MICROBIT_USB_FLASH_I2C_ADDRESS, &b[0], responseLength, false) == MICROBIT_OK)
                return b;
        }

        attempts++;
    }

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
    return transact(command, usbFlashPropertyLength.get(command));
}

/**
 * Destructor.
 */
MicroBitUSBFlashManager::~MicroBitUSBFlashManager()
{

}
