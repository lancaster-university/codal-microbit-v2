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

#include "MicroBitLog.h"
#include "CodalDmesg.h"
#include <new>

#define ARRAY_LEN(array)    (sizeof(array) / sizeof(array[0]))

using namespace codal;

static ManagedString padString(ManagedString s, int digits)
{
    ManagedString zero = "0";
    while(s.length() != digits)
        s = zero + s;

    return s;
}

static void writeNum(char *buf, uint32_t n)
{
    int i = 0;
    int sh = 28;
    while (sh >= 0)
    {
        int d = (n >> sh) & 0xf;
        buf[i++] = d > 9 ? 'A' + d - 10 : '0' + d;
        sh -= 4;
    }
    buf[i] = 0;
}

/**
 * Constructor.
 */
MicroBitLog::MicroBitLog(MicroBitUSBFlashManager &flash, MicroBitPowerManager &power, NRF52Serial &serial) : flash(flash), power(power), serial(serial), cache(flash, CONFIG_MICROBIT_LOG_CACHE_BLOCK_SIZE, 4)
{
    this->journalPages = 0;
    this->status = 0;
    this->journalHead = 0;
    this->startAddress = 0;
    this->journalStart = 0;
    this->dataStart = 0;
    this->headingStart = 0;
    this->headingLength = 0;
    this->headingCount = 0;
    this->logEnd = 0;
    this->headingsChanged = false;
    this->timeStampChanged = false;
    this->rowData = NULL;
    this->timeStampFormat = TimeStampFormat::None;
}

/**
 * Attempt to load an exisitng filesystem, or format a new one if not found.
 */
void MicroBitLog::init()
{
    // If we're already initialized, do nothing. 
    if (status & MICROBIT_LOG_STATUS_INITIALIZED)
        return;

    if (_isPresent())
    {
        // We have a valid file system.
        JournalEntry j;
        journalPages = (dataStart - journalStart) / flash.getPageSize();
        journalHead = journalStart;
        dataEnd = dataStart;

        // Load the last entry in the journal.
        uint32_t journalEntryAddress = journalHead;
        bool valid = false;

        while(journalEntryAddress < dataStart)
        {
            cache.read(journalEntryAddress, j.length, MICROBIT_LOG_JOURNAL_ENTRY_SIZE);

            // If we have a valid reading follwed by an unused entry, we're done.
            if (j.containsOnly(0xFF) && valid)
                break;

            // Parse valid entries. We continue processing to the last valid entry, just in case.
            if (!j.containsOnly(0x00))
            {
                journalHead = journalEntryAddress;
                dataEnd = dataStart + strtoul(j.length, NULL, 16);
                valid = true;
            }

            journalEntryAddress += MICROBIT_LOG_JOURNAL_ENTRY_SIZE;
        }

        // Walk the page indicated by dataEnd, and increment until an unused byte (0xFF) is found.
        uint8_t d = 0;
        while(dataEnd < logEnd)
        {
            cache.read(dataEnd, &d, 1);
            if (d == 0xFF)
                break;

            dataEnd++;
        }

        // Determine if we have any column headers defined
        // If so, parse them.
        uint32_t start = startAddress + sizeof(MicroBitLogMetaData);
        char c;
        
        // Skip any leading zeroes (erased old data)
        cache.read(start, &c, 1);
        while(c == 0)
        {
            start++;
            cache.read(start, &c, 1);
        }

        // Read read until we see a 0xFF character (unused memory)
        uint32_t end = start;
        while(c != 0xff)
        {
            end++;
            cache.read(end, &c, 1);
        }

        headingLength = (int)(end-start);

        // If we have a valid set of headers recorded, parse them in.
        if (headingLength > 0)
        {
            headingStart = start;
            headingCount = 0;

            char *headers = (char *) malloc(headingLength);
            cache.read(start, headers, headingLength);

            // Count the number of comma separated headers.
            // Also terminate each entry as a string as we go.
            for (uint32_t i=0; i<headingLength; i++)
            {
                if (headers[i] == ',' || headers[i] == '\n')
                {
                    headers[i] = 0;
                    headingCount++;
                }
            }

            // Allocate a RAM buffer to hold key/value pairs matching those defined
            rowData = (ColumnEntry *) malloc(sizeof(ColumnEntry) * headingCount);

            // Populate each entry.
            int i=0;
            for (uint32_t h=0; h<headingCount; h++)
            {
                new (&rowData[h]) ColumnEntry;
                rowData[h].key = ManagedString(&headers[i]);
                i = i + rowData[h].key.length() + 1;
            }

            free(headers);
        }

        // We may be full here, but this is still a valid state.
        status |= MICROBIT_LOG_STATUS_INITIALIZED;
        return;
    }
    else
    {
        // No valid file system found. Reformat the physical medium.
        _clear(false);
    }

    // Ensure the data file is visible to end users.
    _setVisibility(true);
}

/**
 * Sets the visibility of the MY_DATA.HTM file on the MICROBIT drive.
 * Only updates the persistent state of this visibility if it has changed.
 *
 * @param visible true to make the file visible, false otherwise.
 */
void MicroBitLog::setVisibility(bool visible)
{
    mutex.wait();
    _setVisibility(visible);
    mutex.notify();
}

/**
 * Sets the visibility of the MY_DATA.HTM file on the MICROBIT drive.
 * Only updates the persistent state of this visibility if it has changed.
 *
 * @param visible true to make the file visible, false otherwise.
 */
void MicroBitLog::_setVisibility(bool visible)
{
    MicroBitUSBFlashConfig config, currentConfig;

    config.fileName = "MY_DATA.HTM";
    config.fileSize = flash.getFlashEnd() - flash.getFlashStart();
    config.visible = visible;
    currentConfig = flash.getConfiguration();

    // If the current configuration matches the required configuration, then there's nothing to do.
    if (config.fileName == currentConfig.fileName && config.fileSize == currentConfig.fileSize && config.visible == currentConfig.visible)
        return;

    // Otherwise update the configuration and remount the drive to ensure the user view is up to date.
    flash.setConfiguration(config, true);
    flash.remount();
}

/**
 * Reset all data stored in persistent storage.
 */
void MicroBitLog::clear(bool fullErase)
{
    mutex.wait();
    _clear(fullErase);
    mutex.notify();
}

/**
 * Reset all data stored in persistent storage.
 */
void MicroBitLog::_clear(bool fullErase)
{
    // Calculate where our metadata should start.
    startAddress = sizeof(header);
    journalPages = CONFIG_MICROBIT_LOG_JOURNAL_SIZE / flash.getPageSize();
    journalStart = startAddress + CONFIG_MICROBIT_LOG_METADATA_SIZE;
    journalHead = journalStart;
    dataStart = journalStart + CONFIG_MICROBIT_LOG_JOURNAL_SIZE;
    dataEnd = dataStart;
    logEnd = flash.getFlashEnd() - sizeof(uint32_t);
    status &= MICROBIT_LOG_STATUS_SERIAL_MIRROR;
    
    // Remove any cached state around column headings
    headingsChanged = false;
    timeStampChanged = false;
    headingStart = 0;
    headingCount = 0;
    headingLength = 0;

    if (rowData)
    {
        free(rowData);
        rowData = NULL;
    }

    // Erase block associated with the FULL indicator. We don't perform a pag eerase here to reduce flash wear.
    uint32_t zero = 0x00000000;
    flash.write(logEnd, &zero, 1);

    // Erase all pages associated with the header, all meta data and the first page of data storage.
    cache.clear();
    for (uint32_t p = flash.getFlashStart(); p <= (fullErase ? logEnd : dataStart); p += flash.getPageSize())
        flash.erase(p);

    // Serialise and write header (if we have one)
    // n.b. we use flash.write() here to avoid unecessary preheating of the cache.
    flash.write(flash.getFlashStart(), (uint32_t *)header, sizeof(header)/4);

    // Generate and write FS metadata
    memcpy(metaData.version, MICROBIT_LOG_VERSION, 18);
    memcpy(metaData.dataStart, "0x00000000\0", 11);
    memcpy(metaData.logEnd, "0x00000000\0", 11);
    memcpy(metaData.daplinkVersion, "0000\0", 5);

    MicroBitVersion versions = power.getVersion();
    ManagedString verStr = padString(versions.daplink > 9999 ? 9999 : versions.daplink, 4);
    memcpy(metaData.daplinkVersion, verStr.toCharArray(), 4);
    writeNum(metaData.dataStart+2, dataStart);
    writeNum(metaData.logEnd+2, logEnd);

    cache.write(startAddress, &metaData, sizeof(metaData));

    // Record that the log is empty
    JournalEntry je;
    cache.write(journalHead, &je, MICROBIT_LOG_JOURNAL_ENTRY_SIZE);

    // Update physical file size and visibility information.
    // If we're doing a full erase, remove the file from view.
    _setVisibility(!fullErase);

    status |= MICROBIT_LOG_STATUS_INITIALIZED;

    // Refresh timestamp settings, to inject the timestamp field into the key value pairs.
    _setTimeStamp(this->timeStampFormat);
}

/**
 * Determines the format of the timestamp data to be added (if any).
 * If requested, time stamps will be automatically added to each row of data
 * as an integer value rounded down to the unit specified.
 * 
 * @param format The format of timestamp to use. 
 */
void MicroBitLog::setTimeStamp(TimeStampFormat format)
{
    mutex.wait();
    _setTimeStamp(format);
    mutex.notify();
}

/**
 * Determines the format of the timestamp data to be added (if any).
 * If requested, time stamps will be automatically added to each row of data
 * as an integer value rounded down to the unit specified.
 * 
 * @param format The format of timestamp to use. 
 */
void MicroBitLog::_setTimeStamp(TimeStampFormat format)
{
    init();

    // Optimize out null operations.
    if (timeStampFormat == format && timeStampFormat == TimeStampFormat::None)
        return;

    // If we do not have a timestamp column associated with the requested time unit, create one.
    ManagedString units;

    switch (format) {

    case TimeStampFormat::None:
        break;

    case TimeStampFormat::Milliseconds:
        units = "milliseconds";
        break;

    case TimeStampFormat::Seconds:
        units = "seconds";
        break;

    case TimeStampFormat::Minutes:
        units = "minutes";
        break;

    case TimeStampFormat::Hours:
        units = "hours";
        break;

    case TimeStampFormat::Days:
        units = "days";
        break;
    }

    timeStampHeading = "Time (" + units + ")";

    // Special case for selecting timestamp headings before the first data is logged.
    // Here, we permit rewriting of Timestamp columns to promote simplicity.
    if (dataStart == dataEnd && headingCount > 0)
    {
        // If this timestamp has already been added. If so, nothing to do.
        if (rowData[0].key == timeStampHeading)
            return;

        // If we've already defined a timestamp heading, remove that timestamp from the list of headings.
        if (this->timeStampFormat != TimeStampFormat::None)
        {
            // Remove the Timestamp column from the list of headings.
            for (uint32_t i=1; i<headingCount; i++)
                rowData[i-1].key = rowData[i].key;

            headingCount--;
        }
    }

    // Update our record of the latest timestamp format that we are using.
    this->timeStampFormat = format;

    // Indicate that we should update column headers the next time a row is logged.
    this->timeStampChanged = true;
}

/**
 * Defines if data logging should also be streamed over the serial port.
 *
 * @param enable True to enable serial port streaming, false to disable.
 */
void MicroBitLog::setSerialMirroring(bool enable)
{
    if (enable)
        status |= MICROBIT_LOG_STATUS_SERIAL_MIRROR;
    else
        status &= ~MICROBIT_LOG_STATUS_SERIAL_MIRROR;
}

/**
 * Creates a new row in the log, ready to be populated by logData()
 * 
 * @return DEVICE_OK on success.
 */
int MicroBitLog::beginRow()
{
    int r;

    mutex.wait();
    r = _beginRow();
    mutex.notify();

    return r;
}


/**
 * Creates a new row in the log, ready to be populated by logData()
 * 
 * @return DEVICE_OK on success.
 */
int MicroBitLog::_beginRow()
{
    init();

    // If beginRow is called during an open transaction, implicity perform an endRow before proceeding.
    if (status & MICROBIT_LOG_STATUS_ROW_STARTED)
        _endRow();

    // Reset all values, ready to populate with a new row.
    for (uint32_t i=0; i<headingCount; i++)
        rowData[i].value = ManagedString();

    // indicate that we've started a new row.
    status |= MICROBIT_LOG_STATUS_ROW_STARTED;

    return DEVICE_OK;
}

/**
 * Populates the current row with the given key/value pair.
 * @param key the name of the key column) to set.
 * @param value the value to insert 
 * 
 * @return DEVICE_OK on success.
 */
int MicroBitLog::logData(const char *key, const char *value)
{
    return logData(ManagedString(key), ManagedString(value));
}

/**
 * Populates the current row with the given key/value pair.
 * @param key the name of the key column) to set.
 * @param value the value to insert 
 *
 * @return DEVICE_OK on success.
 */
int MicroBitLog::logData(ManagedString key, ManagedString value)
{
    int r;

    mutex.wait();
    r = _logData(key, value);
    mutex.notify();

    return r;
}

/**
 * Populates the current row with the given key/value pair.
 * @param key the name of the key column) to set.
 * @param value the value to insert 
 *
 * @return DEVICE_OK on success.
 */
int MicroBitLog::_logData(ManagedString key, ManagedString value)
{
    // Perform lazy instatiation if necessary.
    init();

    // If logData is called before explicitly beginning a row, do so implicitly.
    if (!(status & MICROBIT_LOG_STATUS_ROW_STARTED))
        _beginRow();

    ManagedString k = cleanBuffer(key.toCharArray(), key.length());
    ManagedString v = cleanBuffer(value.toCharArray(), value.length());

    if (k.length())
        key = k;

    if (v.length())
        value = v;

    // Add the given key/value pair into our cumulative row data. 
    bool added = false;
    for (uint32_t i=0; i<headingCount; i++)
    {
        if(rowData[i].key == key)
        {
            rowData[i].value = value;
            added = true;
            break;
        }
    }

    // If the requested heading is not available, add it.
    if (!added)
        addHeading(key, value);

    return DEVICE_OK;
}

/**
 * Complete a row in the log, and pushes to persistent storage.
 * @return DEVICE_OK on success.
 */
int MicroBitLog::endRow()
{
    int r;

    mutex.wait();
    r = _endRow();
    mutex.notify();

    return r;
}

/**
 * Complete a row in the log, and pushes to persistent storage.
 * @return DEVICE_OK on success.
 */
int MicroBitLog::_endRow()
{
    if (!(status & MICROBIT_LOG_STATUS_ROW_STARTED))
        return DEVICE_INVALID_STATE;

    init();

    // Add the timestamp column, if we need one and it does not already exist.
    // Add at the front, unless data has already been written.
    if (timeStampChanged)
    {
        timeStampChanged = false;
        if (timeStampFormat != TimeStampFormat::None)
            addHeading(timeStampHeading, ManagedString::EmptyString, dataStart == dataEnd);
    }

    // Special case the condition where no values are present.
    // We suppress injecting a pointless timestamp in these cases.
    bool validData = false;

    for (uint32_t i=0; i<headingCount; i++)
    {
        if(rowData[i].value != ManagedString::EmptyString)
        {
            validData = true;
            break;
        }
    }

    // Insert timestamp field if requested.
    if (validData && timeStampFormat != TimeStampFormat::None)
    {
        // handle 32 bit overflow and fractional components of timestamp
        CODAL_TIMESTAMP t = system_timer_current_time() / (CODAL_TIMESTAMP)timeStampFormat;
        int billions = t / (CODAL_TIMESTAMP) 1000000000;
        int units = t % (CODAL_TIMESTAMP) 1000000000;
        int fraction = 0;

        if ((int)timeStampFormat > 1)
        {
            fraction = units % 100;
            units = units / 100;
            billions = billions / 100;
        }

        ManagedString u(units);
        ManagedString f(fraction);
        ManagedString s;
        f = padString(f, 2);

        if (billions)
        {
            s = s + billions;
            u = padString(u, 9);
        }

        s = s + u;

        // Add two decimal places for anything other than milliseconds.
        if ((int)timeStampFormat > 1)
            s = s + "." + f;

        _logData(timeStampHeading, s);
    }

    // If new columns have been added since the last row, update persistent storage accordingly.
    ManagedString sep = ",";

    if (headingsChanged)
    {
        // If this is the first time we have logged any headings, place them just after the metadata block
        if (headingStart == 0)
            headingStart = startAddress + sizeof(MicroBitLogMetaData);

        // create new headers
        ManagedString h;
        ManagedBuffer zero(headingLength);

        for (uint32_t i=0; i<headingCount;i++)
        {
            h = h + rowData[i].key;
            if (i + 1 != headingCount)
                h = h + sep;
        }
        h = h + "\n";
        
        cache.write(headingStart, &zero[0], headingLength);
        headingStart += headingLength;
        cache.write(headingStart, h.toCharArray(), h.length());
        headingLength = h.length();

        _logString(h);

        headingsChanged = false;
    }

    // Serialize data to CSV
    ManagedString row;
    bool empty = true;

    for (uint32_t i=0; i<headingCount;i++)
    {
        row = row + rowData[i].value;
         
        if (rowData[i].value.length())
            empty = false;

        if (i + 1 != headingCount)
            row = row + sep;
    }
    row = row + "\n";

    if (!empty)
        _logString(row);

    status &= ~MICROBIT_LOG_STATUS_ROW_STARTED;

    if (status & MICROBIT_LOG_STATUS_FULL)
        return DEVICE_NO_RESOURCES;

    return DEVICE_OK;
}

/**
 * Clean the given buffer of invalid LogFS symbols ("-->" and optionally ",\t\n")
 *
 * @param s the data to clean
 * @param len the number of characters to clean
 * @param removeSeperators if set to false, only "-->" symbols are erased, otherwise ",\t\n" characters are also removed.
 * @return a cleaned version of the string supplied, if any changes are necessary. Otherwise, an empty string is returned.
 */
ManagedString MicroBitLog::cleanBuffer(const char *s, int len, bool removeSeparators)
{
    ManagedString out;

    for (int i=0; i<len; i++)
    {
        if (i+2 < len && s[i] == '-' && s[i+1] == '-' && s[i+2] == '>')
        {
            if (out.length() == 0)
                out = ManagedString(s, len);
            *(char *)(out.toCharArray()+i) = CONFIG_MICROBIT_LOG_INVALID_CHAR_VALUE;
            *(char *)(out.toCharArray()+i+1) = CONFIG_MICROBIT_LOG_INVALID_CHAR_VALUE;
            *(char *)(out.toCharArray()+i+2) = CONFIG_MICROBIT_LOG_INVALID_CHAR_VALUE;
        }

        if (s[i] == '\t' || (removeSeparators && (s[i] == ',' || s[i] == '\n')))
        {
            if (out.length() == 0)
                out = ManagedString(s, len);
            *(char *)(out.toCharArray()+i) = CONFIG_MICROBIT_LOG_INVALID_CHAR_VALUE;
        }
    }
    return out;
}

/**
 * Inject the given row into the log as text, ignoring key/value pairs.
 * @param s the string to inject.
 */
int MicroBitLog::logString(const char *s)
{
    int r;

    mutex.wait();
    r = _logString(s);
    mutex.notify();

    return r;
}

/**
 * Inject the given row into the log as text, ignoring key/value pairs.
 * @param s the string to inject.
 */
int MicroBitLog::_logString(const char *s)
{  
    init();

    uint32_t oldDataEnd = dataEnd;
    uint32_t l = strlen(s);
    const char *data = s;

    // If this is the first log entry written, ensure that the file visibility is activated.
    // (it may have been disabled following a full erase)
    if (dataStart == dataEnd)
        _setVisibility(true);

    // If we can't write a whole line of data, then treat the log as full.
    if (l > logEnd - dataEnd)
    {
        if (!(status & MICROBIT_LOG_STATUS_FULL))
        {
            cache.write(logEnd+1, "FUL", 3);
            status |= MICROBIT_LOG_STATUS_FULL;
        }

        Event(MICROBIT_ID_LOG, MICROBIT_LOG_EVT_LOG_FULL);
        return DEVICE_NO_RESOURCES;
    }

    ManagedString cleaned = cleanBuffer(data, l, false);
    if (cleaned.length())
        data = cleaned.toCharArray();

    // If requested, log the data over the serial port
    if (status & MICROBIT_LOG_STATUS_SERIAL_MIRROR && l > 0)
    {
        serial.send((uint8_t *)data, l-1);
        serial.send((uint8_t *)"\r\n", 2);
    }

    while (l > 0)
    {
        uint32_t spaceOnPage = flash.getPageSize() - (dataEnd % flash.getPageSize());
        //DMESG("SPACE_ON_PAGE: %d", spaceOnPage);
        int lengthToWrite = min(l, spaceOnPage);

        // If we're going to fill (or overspill) the current page, erase the next one ready for use.
        if (spaceOnPage <= l && dataEnd+spaceOnPage < logEnd)
        {
            uint32_t nextPage = ((dataEnd / flash.getPageSize()) + 1) * flash.getPageSize();

            //DMESG("   ERASING PAGE %p", nextPage);
            flash.erase(nextPage);
        }

        // Perform a write through cache update
        //DMESG("   WRITING [ADDRESS: %p] [LENGTH: %d] ", dataEnd, lengthToWrite);
        cache.write(dataEnd, data, lengthToWrite);

        // move on pointers
        dataEnd += lengthToWrite;
        data += lengthToWrite;
        l -= lengthToWrite;
    }

    // Write a new entry into the log journal if we crossed a cache block boundary
    if ((dataEnd / CONFIG_MICROBIT_LOG_CACHE_BLOCK_SIZE) != (oldDataEnd / CONFIG_MICROBIT_LOG_CACHE_BLOCK_SIZE))
    {
        uint32_t oldJournalHead = journalHead;
        oldDataEnd = dataEnd;

        // Record that we've moved on the journal log by one entry
        journalHead += MICROBIT_LOG_JOURNAL_ENTRY_SIZE;

        // If we've moved onto another page, ensure it is erased.
        if (journalHead % flash.getPageSize() == 0)
        {
            //DMESG("JOURNAL PAGE BOUNDARY: %p", journalHead);
            // If we've rolled over the last page, cycle around.
            if (journalHead == dataStart)
            {
                //DMESG("JOURNAL WRAPAROUND");
                journalHead = journalStart;
            }

            //DMESG("ERASING JOURNAL PAGE: %p", journalHead);
            cache.erase(journalHead);
            flash.erase(journalHead);
        }

        // Write journal entry
        JournalEntry je;
        writeNum(je.length, ((dataEnd-dataStart) / CONFIG_MICROBIT_LOG_CACHE_BLOCK_SIZE) * CONFIG_MICROBIT_LOG_CACHE_BLOCK_SIZE);
        cache.write(journalHead, &je, MICROBIT_LOG_JOURNAL_ENTRY_SIZE);

        // Invalidate the old one
        JournalEntry empty;
        empty.clear();
        //DMESG("   INVALIDATING: %p", oldJournalHead);
        cache.write(oldJournalHead, &empty, MICROBIT_LOG_JOURNAL_ENTRY_SIZE);
    }

    // Return NO_RESOURCES if we ran out of FLASH space.
    if (l == 0)
        return DEVICE_OK;

    Event(MICROBIT_ID_LOG, MICROBIT_LOG_EVT_LOG_FULL);
    return DEVICE_NO_RESOURCES;
}

/**
 * Inject the given row into the log as text, ignoring key/value pairs.
 * @param s the string to inject.
 */
int MicroBitLog::logString(ManagedString s)
{
    int r;

    mutex.wait();
    r = _logString(s);
    mutex.notify();

    return r;
}

/**
 * Inject the given row into the log as text, ignoring key/value pairs.
 * @param s the string to inject.
 */
int MicroBitLog::_logString(ManagedString s)
{
    return _logString(s.toCharArray());
}

/**
 * Add the given heading to the list of headings in use. If the heading already exists,
 * this method has no effect.
 * 
 * @param key the heading to add
 * @param value the initial value to add, or ManagedString::EmptyString
 * @param head true to add the given field at the front of the list, false to add at the end.
 */
void MicroBitLog::addHeading(ManagedString key, ManagedString value, bool head)
{
    for (uint32_t i=0; i<headingCount; i++)
        if (rowData[i].key == key)
            return;

    ColumnEntry* newRowData = (ColumnEntry *) malloc(sizeof(ColumnEntry) * (headingCount+1));
    int columnShift = head ? 1 : 0;
    int newColumn = head ? 0 : headingCount;

    for (uint32_t i=0; i<headingCount; i++)
    {
        new (&newRowData[i+columnShift]) ColumnEntry;
        newRowData[i+columnShift].key = rowData[i].key;
        newRowData[i+columnShift].value = rowData[i].value;
        rowData[i].key = ManagedString::EmptyString;
        rowData[i].value = ManagedString::EmptyString;
    }   
    
    if (rowData)
        free(rowData);

    new (&newRowData[newColumn]) ColumnEntry;
    newRowData[newColumn].key = key;
    newRowData[newColumn].value = value;
    headingCount++;

    rowData = newRowData;
    headingsChanged = true;
}

/**
 * Marks an existing Log as invalid. The log will be cleared with the default settings the next time
 * a user attempts to use it. If no valid log is present, this method has no effect.
 */
void MicroBitLog::invalidate()
{
    mutex.wait();
    _invalidate();
    mutex.notify();
}


/**
 * Marks an existing Log as invalid. The log will be cleared with the default settings the next time
 * a user attempts to use it. If no valid log is present, this method has no effect.
 */
void MicroBitLog::_invalidate()
{
    if (_isPresent())
    {

#if CONFIG_ENABLED(CONFIG_MICROBIT_LOG_FULL_ERASE_BY_DEFAULT)
        _clear();
#endif
        MicroBitLogMetaData m;
        memclr(&m, sizeof(MicroBitLogMetaData));

        // Erase the LogFS metadata and trailing FULL indicator.
        flash.write(startAddress, (uint32_t *) &m, sizeof(MicroBitLogMetaData)/4);
        flash.write(logEnd, (uint32_t *) &m, 1);
    }

    status &= ~MICROBIT_LOG_STATUS_INITIALIZED; 
}

/**
 * Determines if a MicroMitLogFS header is present.
 *
 * @return true if a MICROBIT_LOG_VERSION string is present at the expected location, false otherwise.
 */
bool MicroBitLog::isPresent()
{
    bool r;
    mutex.wait();
    r = _isPresent();
    mutex.notify();

    return r;
}

/**
 * Determines if a MicroMitLogFS header is present.
 *
 * @return true if a MICROBIT_LOG_VERSION string is present at the expected location, false otherwise.
 */
bool MicroBitLog::_isPresent()
{
    // Fast path if we;re already initialized.
    if (status & MICROBIT_LOG_STATUS_INITIALIZED)
        return true;

    // Calculate where our metadata should start, and load the data.
    startAddress = sizeof(header);

    // Read the metadata area from flash memory.
    // n.b. we do this using a direct read (rather than via the cache) to avoid preheating the cache with potentially useless data.
    flash.read((uint32_t *)&metaData, startAddress, sizeof(metaData)/4);

    // Ensure data strings are terminated
    metaData.dataStart[ARRAY_LEN(metaData.dataStart) - 1] = 0;
    metaData.logEnd[ARRAY_LEN(metaData.logEnd) - 1] = 0;
    metaData.version[ARRAY_LEN(metaData.version) - 1] = 0;
    metaData.daplinkVersion[ARRAY_LEN(metaData.daplinkVersion) - 1] = 0;

    // Determine if the FS looks valid.
    dataStart = strtoul(metaData.dataStart, NULL, 16);
    logEnd = strtoul(metaData.logEnd, NULL, 16);
    // No need to parse the DAPLink version, as it's not used.
    journalStart = startAddress + CONFIG_MICROBIT_LOG_METADATA_SIZE;

    // Perform some basic validation checks. Load in the state of the file system if things look OK.
    // This intentionally does not check the full LOG_VERSION string, but instead checks if there is a valid
    // version string _preamble_, aka 'UBIT_LOG_FS_V_' to avoid bugs where old flash logs are not detected
    // correctly and erase on a new flash event, as they don't match the version string _exactly_.
    return ( dataStart >= journalStart + flash.getPageSize() &&
        dataStart < logEnd &&
        logEnd < flash.getFlashEnd() &&
        memcmp(metaData.version, MICROBIT_LOG_VERSION, 14) == 0 );
}

/**
 * Determines if this log is full
 *
 * @return true if the log is full, false otherwise.
 */
bool MicroBitLog::isFull()
{
    return (status & MICROBIT_LOG_STATUS_FULL);
}

/**
 * Get the length of the recorded data
 * @param format the data format
 *          DataFormat::HTMLHeader = 0,   - The HTML header without the data
 *          DataFormat::HTML = 1,               - The entire HTML file
 *          DataFormat::CSV = 2                   - CSV data
 * @return the length of the recorded data
 */
uint32_t MicroBitLog::getDataLength(DataFormat format)
{
    uint32_t r = 0;
    mutex.wait();
    init();
    uint32_t hdr = sizeof(header);
    uint32_t mtr = sizeof(MicroBitLogMetaData);
    uint32_t csv = dataEnd - dataStart;
    switch (format)
    {
        case DataFormat::HTMLHeader:
            r = hdr + mtr + 1;
            break;
        case DataFormat::HTML:
            r = hdr + mtr + csv + 1;
            break;
        case DataFormat::CSV:
            r = csv;
            break;
    }
    mutex.notify();
    return r;
}

/**
 * Read the recorded data
 * @param data pointer to memory to store the data
 * @param index  the index into the data
 * @param len length of the data to fetch
 * @param format the data format
 *          DataFormat::HTMLHeader = 0,   - The HTML header without data
 *          DataFormat::HTML = 1,               - The entire HTML file with data
 *          DataFormat::CSV = 2                   - CSV data
 * @param length expected complete length returned from getDataLength
 * @return DEVICE_OK on success; DEVICE_INVALID_PARAMETER if data is not available for the request
 */
int MicroBitLog::readData(void *data, uint32_t index, uint32_t len, DataFormat format, uint32_t length)
{
    int r;
    mutex.wait();
    r = _readData((uint8_t *) data, index, len, format, length);
    mutex.notify();
    return r;
}

int MicroBitLog::_readData(uint8_t *data, uint32_t index, uint32_t len, DataFormat format, uint32_t length)
{
    int r = DEVICE_OK;
    
    init();
    
    uint32_t hdr = sizeof(header);
    uint32_t mtr = sizeof(MicroBitLogMetaData);

    // Check if there is less data than expected
    uint32_t dataMax = dataEnd - dataStart;
    uint32_t dataLen = dataMax;
    switch (format)
    {
        case DataFormat::HTML:
            dataLen = length - hdr - mtr - 1;
            break;
        case DataFormat::CSV:
            dataLen = length;
            break;
        default:
            break;
    }
    if ( dataLen > dataMax)
        return DEVICE_INVALID_PARAMETER;

    // Generate metadata without journal pages
    // logEnd is reduced by the same amount as dataStart
    // TODO Do we want the journal pages in the HTML?
    MicroBitLogMetaData meta = metaData;
    writeNum(meta.dataStart+2, hdr + mtr);
    writeNum(meta.logEnd+2, logEnd - (dataStart - hdr - mtr));

    uint8_t end = 0xFF;
    
    uint32_t pos = 0;
                      
    switch (format)
    {
        case DataFormat::HTMLHeader:
            _readSource( data, index, len, pos, header, 0, hdr);
            _readSource( data, index, len, pos, &meta,  0, mtr);
            _readSource( data, index, len, pos, &end,   0, sizeof(end));
            break;
        case DataFormat::HTML:
            _readSource( data, index, len, pos, header, 0, hdr);
            _readSource( data, index, len, pos, &meta,  0, mtr);
            r = _readSource( data, index, len, pos, NULL, dataStart, dataLen);
            if (r == DEVICE_OK)
              _readSource( data, index, len, pos, &end, 0, sizeof(end));
            break;
        case DataFormat::CSV:
            r = _readSource( data, index, len, pos, NULL, dataStart, dataLen);
            break;
    }
    return r;
}

/**
 * Read the source data from local memory or interface flash
 * @param data pointer reference to memory to store the data
 * @param index  reference to the index into the data
 * @param len reference to the length of the data to fetch
 * @param srcIndex reference to the index where the source data should begin
 * @param srcPtr pointer to  source data in local memory. NULL to use srcAddress
 * @param srcAddress address of source data in interface flash. Ignored if srcPtr is not NULL.
 * @param srcLen the length of the source data
 * @return DEVICE_OK on success; DEVICE_INVALID_PARAMETER if data is not available for the request
 * @note On success, the referenced pointers, indices and lengths are updated ready for the next call
 */
int MicroBitLog::_readSource( uint8_t *&data, uint32_t &index, uint32_t &len, uint32_t &srcIndex, const void *srcPtr, uint32_t srcAddress, uint32_t srcLen)
{
    int r = DEVICE_OK;
    uint32_t next = srcIndex + srcLen;
    uint32_t length = index < next ? next - index : 0;
    if ( length > len)
        length = len;

    if ( length)
    {
        if ( srcPtr)
            memcpy(data, (const uint8_t *) srcPtr + (index - srcIndex), length);
        else
            r = cache.read( srcAddress + (index - srcIndex), data, length);
    }

    if ( r == DEVICE_OK)
    {
        data    += length;
        index   += length;
        len     -= length;

        srcIndex = next;
    }
    return r;
}

/**
 * Destructor.
 */
MicroBitLog::~MicroBitLog()
{

}

const uint8_t MicroBitLog::header[2048] = {0x3c,0x6d,0x65,0x74,0x61,0x20,0x63,0x68,0x61,0x72,0x73,0x65,0x74,0x3d,0x75,0x74,0x66,0x2d,0x38,0x3e,0x3c,0x73,0x74,0x79,0x6c,0x65,0x3e,0x2e,0x62,0x62,0x7b,0x64,0x69,0x73,0x70,0x6c,0x61,0x79,0x3a,0x66,0x6c,0x65,0x78,0x7d,0x2e,0x62,0x62,0x3e,0x2a,0x2b,0x2a,0x7b,0x6d,0x61,0x72,0x67,0x69,0x6e,0x2d,0x6c,0x65,0x66,0x74,0x3a,0x31,0x30,0x70,0x78,0x7d,0x62,0x6f,0x64,0x79,0x7b,0x66,0x6f,0x6e,0x74,0x2d,0x66,0x61,0x6d,0x69,0x6c,0x79,0x3a,0x73,0x61,0x6e,0x73,0x2d,0x73,0x65,0x72,0x69,0x66,0x3b,0x6d,0x61,0x72,0x67,0x69,0x6e,0x3a,0x31,0x65,0x6d,0x7d,0x74,0x61,0x62,0x6c,0x65,0x7b,0x62,0x6f,0x72,0x64,0x65,0x72,0x2d,0x63,0x6f,0x6c,0x6c,0x61,0x70,0x73,0x65,0x3a,0x63,0x6f,0x6c,0x6c,0x61,0x70,0x73,0x65,0x3b,0x6d,0x61,0x72,0x67,0x69,0x6e,0x2d,0x74,0x6f,0x70,0x3a,0x31,0x65,0x6d,0x3b,0x74,0x65,0x78,0x74,0x2d,0x61,0x6c,0x69,0x67,0x6e,0x3a,0x72,0x69,0x67,0x68,0x74,0x7d,0x74,0x72,0x3a,0x66,0x69,0x72,0x73,0x74,0x2d,0x63,0x68,0x69,0x6c,0x64,0x7b,0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x37,0x30,0x30,0x7d,0x74,0x64,0x7b,0x62,0x6f,0x72,0x64,0x65,0x72,0x3a,0x31,0x70,0x78,0x20,0x73,0x6f,0x6c,0x69,0x64,0x20,0x23,0x64,0x64,0x64,0x3b,0x70,0x61,0x64,0x64,0x69,0x6e,0x67,0x3a,0x38,0x70,0x78,0x3b,0x6d,0x69,0x6e,0x2d,0x77,0x69,0x64,0x74,0x68,0x3a,0x38,0x63,0x68,0x7d,0x69,0x66,0x72,0x61,0x6d,0x65,0x7b,0x64,0x69,0x73,0x70,0x6c,0x61,0x79,0x3a,0x6e,0x6f,0x6e,0x65,0x7d,0x3c,0x2f,0x73,0x74,0x79,0x6c,0x65,0x3e,0x3c,0x6c,0x69,0x6e,0x6b,0x20,0x72,0x65,0x6c,0x3d,0x73,0x74,0x79,0x6c,0x65,0x73,0x68,0x65,0x65,0x74,0x20,0x68,0x72,0x65,0x66,0x3d,0x68,0x74,0x74,0x70,0x73,0x3a,0x2f,0x2f,0x6d,0x69,0x63,0x72,0x6f,0x62,0x69,0x74,0x2e,0x6f,0x72,0x67,0x2f,0x64,0x6c,0x2f,0x32,0x2f,0x64,0x6c,0x2e,0x63,0x73,0x73,0x3e,0x3c,0x73,0x63,0x72,0x69,0x70,0x74,0x3e,0x6c,0x65,0x74,0x20,0x77,0x3d,0x77,0x69,0x6e,0x64,0x6f,0x77,0x2c,0x64,0x3d,0x64,0x6f,0x63,0x75,0x6d,0x65,0x6e,0x74,0x2c,0x6c,0x3d,0x77,0x2e,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x2c,0x6e,0x3d,0x6e,0x75,0x6c,0x6c,0x2c,0x63,0x73,0x76,0x3d,0x22,0x22,0x2c,0x74,0x61,0x67,0x3d,0x64,0x2e,0x63,0x72,0x65,0x61,0x74,0x65,0x45,0x6c,0x65,0x6d,0x65,0x6e,0x74,0x2e,0x62,0x69,0x6e,0x64,0x28,0x64,0x29,0x3b,0x77,0x2e,0x64,0x6c,0x3d,0x7b,0x64,0x6f,0x77,0x6e,0x6c,0x6f,0x61,0x64,0x3a,0x66,0x75,0x6e,0x63,0x74,0x69,0x6f,0x6e,0x28,0x29,0x7b,0x6c,0x65,0x74,0x20,0x65,0x3d,0x74,0x61,0x67,0x28,0x22,0x61,0x22,0x29,0x3b,0x65,0x2e,0x64,0x6f,0x77,0x6e,0x6c,0x6f,0x61,0x64,0x3d,0x22,0x6d,0x69,0x63,0x72,0x6f,0x62,0x69,0x74,0x2e,0x63,0x73,0x76,0x22,0x2c,0x65,0x2e,0x68,0x72,0x65,0x66,0x3d,0x55,0x52,0x4c,0x2e,0x63,0x72,0x65,0x61,0x74,0x65,0x4f,0x62,0x6a,0x65,0x63,0x74,0x55,0x52,0x4c,0x28,0x6e,0x65,0x77,0x20,0x42,0x6c,0x6f,0x62,0x28,0x5b,0x63,0x73,0x76,0x5d,0x2c,0x7b,0x74,0x79,0x70,0x65,0x3a,0x22,0x74,0x65,0x78,0x74,0x2f,0x70,0x6c,0x61,0x69,0x6e,0x22,0x7d,0x29,0x29,0x2c,0x65,0x2e,0x63,0x6c,0x69,0x63,0x6b,0x28,0x29,0x2c,0x65,0x2e,0x72,0x65,0x6d,0x6f,0x76,0x65,0x28,0x29,0x7d,0x2c,0x63,0x6f,0x70,0x79,0x3a,0x66,0x75,0x6e,0x63,0x74,0x69,0x6f,0x6e,0x28,0x29,0x7b,0x6e,0x61,0x76,0x69,0x67,0x61,0x74,0x6f,0x72,0x2e,0x63,0x6c,0x69,0x70,0x62,0x6f,0x61,0x72,0x64,0x2e,0x77,0x72,0x69,0x74,0x65,0x54,0x65,0x78,0x74,0x28,0x63,0x73,0x76,0x2e,0x72,0x65,0x70,0x6c,0x61,0x63,0x65,0x28,0x2f,0x5c,0x2c,0x2f,0x67,0x2c,0x22,0x5c,0x74,0x22,0x29,0x29,0x7d,0x2c,0x75,0x70,0x64,0x61,0x74,0x65,0x3a,0x61,0x6c,0x65,0x72,0x74,0x2e,0x62,0x69,0x6e,0x64,0x28,0x6e,0x2c,0x22,0x55,0x6e,0x70,0x6c,0x75,0x67,0x20,0x79,0x6f,0x75,0x72,0x20,0x6d,0x69,0x63,0x72,0x6f,0x3a,0x62,0x69,0x74,0x2c,0x20,0x74,0x68,0x65,0x6e,0x20,0x70,0x6c,0x75,0x67,0x20,0x69,0x74,0x20,0x62,0x61,0x63,0x6b,0x20,0x69,0x6e,0x20,0x61,0x6e,0x64,0x20,0x77,0x61,0x69,0x74,0x22,0x29,0x2c,0x63,0x6c,0x65,0x61,0x72,0x3a,0x61,0x6c,0x65,0x72,0x74,0x2e,0x62,0x69,0x6e,0x64,0x28,0x6e,0x2c,0x22,0x54,0x68,0x65,0x20,0x6c,0x6f,0x67,0x20,0x69,0x73,0x20,0x63,0x6c,0x65,0x61,0x72,0x65,0x64,0x20,0x77,0x68,0x65,0x6e,0x20,0x79,0x6f,0x75,0x20,0x72,0x65,0x66,0x6c,0x61,0x73,0x68,0x20,0x79,0x6f,0x75,0x72,0x20,0x6d,0x69,0x63,0x72,0x6f,0x3a,0x62,0x69,0x74,0x22,0x29,0x2c,0x6c,0x6f,0x61,0x64,0x3a,0x66,0x75,0x6e,0x63,0x74,0x69,0x6f,0x6e,0x28,0x29,0x7b,0x6c,0x65,0x74,0x20,0x61,0x3d,0x64,0x2e,0x71,0x75,0x65,0x72,0x79,0x53,0x65,0x6c,0x65,0x63,0x74,0x6f,0x72,0x28,0x22,0x23,0x77,0x22,0x29,0x2c,0x69,0x3d,0x64,0x2e,0x64,0x6f,0x63,0x75,0x6d,0x65,0x6e,0x74,0x45,0x6c,0x65,0x6d,0x65,0x6e,0x74,0x2e,0x6f,0x75,0x74,0x65,0x72,0x48,0x54,0x4d,0x4c,0x2e,0x73,0x70,0x6c,0x69,0x74,0x28,0x22,0x46,0x53,0x5f,0x53,0x54,0x41,0x52,0x54,0x22,0x29,0x5b,0x32,0x5d,0x3b,0x69,0x66,0x28,0x2f,0x5e,0x55,0x42,0x49,0x54,0x5f,0x4c,0x4f,0x47,0x5f,0x46,0x53,0x5f,0x56,0x5f,0x30,0x30,0x32,0x2f,0x2e,0x74,0x65,0x73,0x74,0x28,0x69,0x29,0x29,0x7b,0x6c,0x65,0x74,0x20,0x74,0x3d,0x70,0x61,0x72,0x73,0x65,0x49,0x6e,0x74,0x3b,0x74,0x68,0x69,0x73,0x2e,0x64,0x61,0x70,0x56,0x65,0x72,0x3d,0x74,0x28,0x69,0x2e,0x73,0x75,0x62,0x73,0x74,0x72,0x28,0x34,0x30,0x2c,0x34,0x29,0x2c,0x31,0x30,0x29,0x3b,0x76,0x61,0x72,0x20,0x6e,0x3d,0x74,0x28,0x69,0x2e,0x73,0x75,0x62,0x73,0x74,0x72,0x28,0x32,0x39,0x2c,0x31,0x30,0x29,0x2c,0x31,0x36,0x29,0x2d,0x32,0x30,0x34,0x38,0x3b,0x6c,0x65,0x74,0x20,0x65,0x3d,0x30,0x3b,0x66,0x6f,0x72,0x28,0x3b,0x36,0x35,0x35,0x33,0x33,0x21,0x3d,0x69,0x2e,0x63,0x68,0x61,0x72,0x43,0x6f,0x64,0x65,0x41,0x74,0x28,0x6e,0x2b,0x65,0x29,0x3b,0x29,0x65,0x2b,0x2b,0x3b,0x63,0x73,0x76,0x3d,0x69,0x2e,0x73,0x75,0x62,0x73,0x74,0x72,0x28,0x6e,0x2c,0x65,0x29,0x3b,0x6c,0x65,0x74,0x20,0x72,0x3d,0x30,0x3b,0x66,0x6f,0x72,0x28,0x6c,0x65,0x74,0x20,0x65,0x3d,0x30,0x3b,0x65,0x3c,0x69,0x2e,0x6c,0x65,0x6e,0x67,0x74,0x68,0x3b,0x2b,0x2b,0x65,0x29,0x72,0x3d,0x33,0x31,0x2a,0x72,0x2b,0x69,0x2e,0x63,0x68,0x61,0x72,0x43,0x6f,0x64,0x65,0x41,0x74,0x28,0x65,0x29,0x2c,0x72,0x7c,0x3d,0x30,0x3b,0x76,0x61,0x72,0x20,0x6f,0x3d,0x6c,0x2e,0x68,0x72,0x65,0x66,0x2e,0x73,0x70,0x6c,0x69,0x74,0x28,0x22,0x3f,0x22,0x29,0x5b,0x31,0x5d,0x3b,0x69,0x66,0x28,0x76,0x6f,0x69,0x64,0x20,0x30,0x21,0x3d,0x3d,0x6f,0x29,0x6f,0x21,0x3d,0x72,0x26,0x26,0x70,0x61,0x72,0x65,0x6e,0x74,0x2e,0x70,0x6f,0x73,0x74,0x4d,0x65,0x73,0x73,0x61,0x67,0x65,0x28,0x22,0x64,0x69,0x66,0x66,0x22,0x2c,0x22,0x2a,0x22,0x29,0x3b,0x65,0x6c,0x73,0x65,0x7b,0x6f,0x3d,0x74,0x28,0x69,0x2e,0x73,0x75,0x62,0x73,0x74,0x72,0x28,0x31,0x38,0x2c,0x31,0x30,0x29,0x2c,0x31,0x36,0x29,0x3b,0x22,0x46,0x55,0x4c,0x22,0x3d,0x3d,0x3d,0x69,0x2e,0x73,0x75,0x62,0x73,0x74,0x72,0x28,0x6f,0x2d,0x32,0x30,0x34,0x38,0x2b,0x31,0x2c,0x33,0x29,0x26,0x26,0x28,0x61,0x2e,0x61,0x70,0x70,0x65,0x6e,0x64,0x43,0x68,0x69,0x6c,0x64,0x28,0x74,0x61,0x67,0x28,0x22,0x70,0x22,0x29,0x29,0x2e,0x69,0x6e,0x6e,0x65,0x72,0x54,0x65,0x78,0x74,0x3d,0x22,0x4c,0x4f,0x47,0x20,0x46,0x55,0x4c,0x4c,0x22,0x29,0x3b,0x6c,0x65,0x74,0x20,0x6e,0x3d,0x61,0x2e,0x61,0x70,0x70,0x65,0x6e,0x64,0x43,0x68,0x69,0x6c,0x64,0x28,0x74,0x61,0x67,0x28,0x22,0x74,0x61,0x62,0x6c,0x65,0x22,0x29,0x29,0x3b,0x63,0x73,0x76,0x2e,0x73,0x70,0x6c,0x69,0x74,0x28,0x22,0x5c,0x6e,0x22,0x29,0x2e,0x66,0x6f,0x72,0x45,0x61,0x63,0x68,0x28,0x66,0x75,0x6e,0x63,0x74,0x69,0x6f,0x6e,0x28,0x65,0x29,0x7b,0x6c,0x65,0x74,0x20,0x74,0x3d,0x6e,0x2e,0x69,0x6e,0x73,0x65,0x72,0x74,0x52,0x6f,0x77,0x28,0x29,0x3b,0x65,0x26,0x26,0x65,0x2e,0x73,0x70,0x6c,0x69,0x74,0x28,0x22,0x2c,0x22,0x29,0x2e,0x66,0x6f,0x72,0x45,0x61,0x63,0x68,0x28,0x66,0x75,0x6e,0x63,0x74,0x69,0x6f,0x6e,0x28,0x65,0x29,0x7b,0x74,0x2e,0x69,0x6e,0x73,0x65,0x72,0x74,0x43,0x65,0x6c,0x6c,0x28,0x29,0x2e,0x69,0x6e,0x6e,0x65,0x72,0x54,0x65,0x78,0x74,0x3d,0x65,0x7d,0x29,0x7d,0x29,0x2c,0x77,0x2e,0x6f,0x6e,0x6d,0x65,0x73,0x73,0x61,0x67,0x65,0x3d,0x66,0x75,0x6e,0x63,0x74,0x69,0x6f,0x6e,0x28,0x65,0x29,0x7b,0x22,0x64,0x69,0x66,0x66,0x22,0x3d,0x3d,0x65,0x2e,0x64,0x61,0x74,0x61,0x26,0x26,0x6c,0x2e,0x72,0x65,0x6c,0x6f,0x61,0x64,0x28,0x29,0x7d,0x3b,0x6c,0x65,0x74,0x20,0x65,0x3b,0x73,0x65,0x74,0x49,0x6e,0x74,0x65,0x72,0x76,0x61,0x6c,0x28,0x66,0x75,0x6e,0x63,0x74,0x69,0x6f,0x6e,0x28,0x29,0x7b,0x65,0x26,0x26,0x65,0x2e,0x72,0x65,0x6d,0x6f,0x76,0x65,0x28,0x29,0x2c,0x65,0x3d,0x61,0x2e,0x61,0x70,0x70,0x65,0x6e,0x64,0x43,0x68,0x69,0x6c,0x64,0x28,0x74,0x61,0x67,0x28,0x22,0x69,0x66,0x72,0x61,0x6d,0x65,0x22,0x29,0x29,0x2c,0x65,0x2e,0x73,0x72,0x63,0x3d,0x6c,0x2e,0x68,0x72,0x65,0x66,0x2b,0x22,0x3f,0x22,0x2b,0x72,0x7d,0x2c,0x35,0x65,0x33,0x29,0x7d,0x7d,0x7d,0x7d,0x3c,0x2f,0x73,0x63,0x72,0x69,0x70,0x74,0x3e,0x3c,0x73,0x63,0x72,0x69,0x70,0x74,0x20,0x73,0x72,0x63,0x3d,0x68,0x74,0x74,0x70,0x73,0x3a,0x2f,0x2f,0x6d,0x69,0x63,0x72,0x6f,0x62,0x69,0x74,0x2e,0x6f,0x72,0x67,0x2f,0x64,0x6c,0x2f,0x32,0x2f,0x64,0x6c,0x2e,0x6a,0x73,0x3e,0x3c,0x2f,0x73,0x63,0x72,0x69,0x70,0x74,0x3e,0x3c,0x74,0x69,0x74,0x6c,0x65,0x3e,0x6d,0x69,0x63,0x72,0x6f,0x3a,0x62,0x69,0x74,0x20,0x64,0x61,0x74,0x61,0x20,0x6c,0x6f,0x67,0x3c,0x2f,0x74,0x69,0x74,0x6c,0x65,0x3e,0x3c,0x62,0x6f,0x64,0x79,0x20,0x6f,0x6e,0x6c,0x6f,0x61,0x64,0x3d,0x64,0x6c,0x2e,0x6c,0x6f,0x61,0x64,0x28,0x29,0x3e,0x3c,0x64,0x69,0x76,0x20,0x69,0x64,0x3d,0x77,0x3e,0x3c,0x68,0x31,0x3e,0x6d,0x69,0x63,0x72,0x6f,0x3a,0x62,0x69,0x74,0x20,0x64,0x61,0x74,0x61,0x20,0x6c,0x6f,0x67,0x3c,0x2f,0x68,0x31,0x3e,0x3c,0x64,0x69,0x76,0x20,0x63,0x6c,0x61,0x73,0x73,0x3d,0x62,0x62,0x3e,0x3c,0x62,0x75,0x74,0x74,0x6f,0x6e,0x20,0x6f,0x6e,0x63,0x6c,0x69,0x63,0x6b,0x3d,0x64,0x6c,0x2e,0x64,0x6f,0x77,0x6e,0x6c,0x6f,0x61,0x64,0x28,0x29,0x3e,0x44,0x6f,0x77,0x6e,0x6c,0x6f,0x61,0x64,0x3c,0x2f,0x62,0x75,0x74,0x74,0x6f,0x6e,0x3e,0x3c,0x62,0x75,0x74,0x74,0x6f,0x6e,0x20,0x6f,0x6e,0x63,0x6c,0x69,0x63,0x6b,0x3d,0x64,0x6c,0x2e,0x63,0x6f,0x70,0x79,0x28,0x29,0x3e,0x43,0x6f,0x70,0x79,0x3c,0x2f,0x62,0x75,0x74,0x74,0x6f,0x6e,0x3e,0x3c,0x62,0x75,0x74,0x74,0x6f,0x6e,0x20,0x6f,0x6e,0x63,0x6c,0x69,0x63,0x6b,0x3d,0x64,0x6c,0x2e,0x75,0x70,0x64,0x61,0x74,0x65,0x28,0x29,0x3e,0x55,0x70,0x64,0x61,0x74,0x65,0x20,0x64,0x61,0x74,0x61,0x26,0x6d,0x6c,0x64,0x72,0x3b,0x3c,0x2f,0x62,0x75,0x74,0x74,0x6f,0x6e,0x3e,0x3c,0x62,0x75,0x74,0x74,0x6f,0x6e,0x20,0x6f,0x6e,0x63,0x6c,0x69,0x63,0x6b,0x3d,0x64,0x6c,0x2e,0x63,0x6c,0x65,0x61,0x72,0x28,0x29,0x3e,0x43,0x6c,0x65,0x61,0x72,0x20,0x6c,0x6f,0x67,0x26,0x6d,0x6c,0x64,0x72,0x3b,0x3c,0x2f,0x62,0x75,0x74,0x74,0x6f,0x6e,0x3e,0x3c,0x2f,0x64,0x69,0x76,0x3e,0x3c,0x70,0x20,0x69,0x64,0x3d,0x76,0x3e,0x4f,0x66,0x66,0x6c,0x69,0x6e,0x65,0x3a,0x20,0x6e,0x6f,0x20,0x76,0x69,0x73,0x75,0x61,0x6c,0x20,0x70,0x72,0x65,0x76,0x69,0x65,0x77,0x3c,0x2f,0x64,0x69,0x76,0x3e,0x20,0x3c,0x21,0x2d,0x2d,0x46,0x53,0x5f,0x53,0x54,0x41,0x52,0x54};
