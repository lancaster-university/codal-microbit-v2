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

/*
 * Log file system format:
 *
 * H = strlen(MICROBIT_LOG_HEADER) rounded up to page boundary.
 * N = (dataStart - H * PAGE_SIZE) / PAGE_SIZE
 * M = logLength - (N+H)*PAGE_SIZE
 * 
 * H x Header pages.
 * +===========================+
 * |     PREEFINED-HEADER      |
 * +===========================+
 * 
 * +===========================+
 * |                           |
 * |    MicroBitLogMetaData    |
 * |       (40 bytes)          |
 * |                           | 
 * +---------------------------+
 * |                           |
 * |     0x00 Init Region      |
 * |     (variable length)     |
 * |                           |
 * +---------------------------+
 * |                           |
 * | Comma separated Log Keys  |
 * |     (variable length)     |
 * |                           |
 * +---------------------------|
 * |                           |
 * |     0xFF Init Region      |
 * |     (variable length)     |
 * |                           |
 * +===========================+
 * 
 *
 * N x Journal Pages:
 * +===========================+
 * |                           |
 * |     0x00 Init Region      |
 * |     (variable length)     |
 * |                           |
 * +---------------------------+
 * |                           |
 * |      Logfile length       |
 * |         (12 bytes)        |
 * |                           |
 * +---------------------------|
 * |                           |
 * |     0xFF Init Region      |
 * |     (variable length)     |
 * |                           |
 * +===========================+
 * 
 * M x pages
 * +===========================+
 * |                           |
 * |         Log Data          |
 * |                           |
 * +---------------------------|
 * |                           |
 * |     0xFF Init Region      |
 * |     (variable length)     |
 * |                           |
 * +===========================+
 * 
 */

#ifndef MICROBIT_LOG_H
#define MICROBIT_LOG_H

#include "MicroBitUSBFlashManager.h"
#include "FSCache.h"
#include "NRF52Serial.h"
#include "ManagedString.h"

#ifndef CONFIG_MICROBIT_LOG_METADATA_SIZE
#define CONFIG_MICROBIT_LOG_METADATA_SIZE      2048
#endif

#ifndef CONFIG_MICROBIT_LOG_JOURNAL_SIZE
#define CONFIG_MICROBIT_LOG_JOURNAL_SIZE       4096
#endif


#ifndef CONFIG_MICROBIT_LOG_CACHE_BLOCK_SIZE
#define CONFIG_MICROBIT_LOG_CACHE_BLOCK_SIZE    256
#endif

#ifndef CONFIG_MICROBIT_LOG_FULL_ERASE_BY_DEFAULT
#define CONFIG_MICROBIT_LOG_FULL_ERASE_BY_DEFAULT    false
#endif

#ifndef CONFIG_MICROBIT_LOG_INVALID_CHAR_VALUE
#define CONFIG_MICROBIT_LOG_INVALID_CHAR_VALUE  '_'
#endif

#define MICROBIT_LOG_VERSION                "UBIT_LOG_FS_V_002\n"           // MUST be 18 characters.
#define MICROBIT_LOG_JOURNAL_ENTRY_SIZE     8

#define MICROBIT_LOG_STATUS_INITIALIZED     0x0001
#define MICROBIT_LOG_STATUS_ROW_STARTED     0x0002
#define MICROBIT_LOG_STATUS_FULL            0x0004
#define MICROBIT_LOG_STATUS_SERIAL_MIRROR   0x0008


#define MICROBIT_LOG_EVT_LOG_FULL           1

namespace codal
{
    struct MicroBitLogMetaData
    {
        char        version[18];             // MICROBIT_LOG_VERSION
        char        logEnd[11];              // 32 bit HEX representation containing end address of available storage (e.g. "0x0000FFFF\0")
        char        dataStart[11];           // 32 bit HEX representation of logical start address of data file (e.g. "0x00000200\0")
        char        daplinkVersion[5];       // 4 ASCII characters for the DAPLink version (e.g. "0255\0")
    };

    class ColumnEntry
    {
        public:
        ManagedString key;
        ManagedString value;
    };

    
    enum class TimeStampFormat
    {
        None = 0,
        Milliseconds = 1,
        Seconds = 10,
        Minutes = 600,
        Hours = 36000,
        Days = 864000
    };

    struct JournalEntry
    {
        char        length[MICROBIT_LOG_JOURNAL_ENTRY_SIZE];
        char        null;

        JournalEntry()
        {
            memcpy(length, "00000000", MICROBIT_LOG_JOURNAL_ENTRY_SIZE);
            null = 0;
        }

        void clear()
        {
            memset(length, 0, MICROBIT_LOG_JOURNAL_ENTRY_SIZE);
        }

        bool containsOnly(uint8_t value)
        {
            for (int i=0; i<MICROBIT_LOG_JOURNAL_ENTRY_SIZE;i++)
                if (length[i] != value)
                    return false;

            return true;
        }
    };


    enum class DataFormat
    {
        HTMLHeader = 0,   // The HTML header without the data
        HTML = 1,         // The entire HTML file
        CSV = 2           // CSV data
    };

    /**
     * Class definition for MicroBitLog. A simple text only, append only, single file log file system.
     * Also contains a key/value pair abstraction to enable dynamic creation of CSV based logfiles.
     */
    class MicroBitLog 
    {
        private:
        MicroBitUSBFlashManager         &flash;             // Non-volatile memory controller to use for storage.
        MicroBitPowerManager            &power;             // To obtain the Interface chip firmware (DAPLink) version.
        NRF52Serial                     &serial;            // Reference to serial port used for data mirroring.
        FSCache                         cache;              // Write through RAM cache.
        uint32_t                        status;             // Status flags.
        FiberLock                       mutex;              // Mutual exclusion primitive to serialise APi calls.

        uint32_t                        startAddress;       // Logical address of the start of the Log file system.
        uint32_t                        journalPages;       // Number of physical pages allocated to journalling.
        uint32_t                        journalStart;       // logical address of the start of the journal section.
        uint32_t                        journalHead;        // Logical address of the last valid journal entry.
        uint32_t                        dataStart;          // Logical address of the start of the Data section.
        uint32_t                        dataEnd;            // Logical address of the end of valid data.
        uint32_t                        logEnd;             // Logical address of the end of the file system space.
        uint32_t                        headingStart;       // Logical address of the start of the column header data. Zero if no data is present.
        uint32_t                        headingLength;      // The length (in bytes) of the column header data.
        uint32_t                        headingCount;       // Total number of headings in the current log.
        bool                            headingsChanged;    // Flag to indicate if a row has been added that contains new columns.
        bool                            timeStampChanged;   // Flag to indicate if a timestamp format has changed.

        struct ColumnEntry*             rowData;            // Collection of key/value pairs. Used to accumulate each data row.
        struct MicroBitLogMetaData      metaData;           // Snapshot of the metadata held in flash storage.
        TimeStampFormat                 timeStampFormat;    // The format of timestamp to log on each row.
        ManagedString                   timeStampHeading;   // The title of the timestamp column, including units.

        const static uint8_t            header[2048];       // static header to prepend to FS in physical storage.

        public:

        /**
         * Constructor.
         */
        MicroBitLog(MicroBitUSBFlashManager &flash, MicroBitPowerManager &power, NRF52Serial &serial);

        /**
         * Destructor.
         */
        ~MicroBitLog();

        /**
         * Determines if a MicroBitLogFS header is present.
         *
         * @return true if a MICROBIT_LOG_VERSION string is present at the expected location, false otherwise.
         */
        bool isPresent();

        /**
         * Determines if this log is full
         *
         * @return true if the log is full, false otherwise.
         */
        bool isFull();

        /**
         * Sets the visibility of the MY_DATA.HTM file on the MICROBIT drive.
         * Only updates the persistent state of this visibility if it has changed.
         *
         * @param visible true to make the file visible, false otherwise.
         */
        void setVisibility(bool visible);

        /**
         * Clears the current log, including any previously defined keys.
         * By default a "fast format" approach is typically adopted, where only the FS metadata is reset. Any previously
         * stored user data will not be visible, but could still be extracted using forensic techniques.
         * A full erase option is is supported that elimates all trace of user data, but this is not recommended
         * unless strictly necessary as it will take a long time to complete and promotes excessive flash wear.
         *
         * @param fullErase if set to true, all data will be hard erased from storage.
         */
        void clear(bool fullErase = CONFIG_MICROBIT_LOG_FULL_ERASE_BY_DEFAULT);

        /**
         * Marks an existing Log as invalid. The log will be cleared with the default settings the next time
         * a user attempts to use it. If no valid log is present, this method has no effect.
         */
        void invalidate();


        /**
         * Determines the format of the timestamp data to be added (if any).
         * If requested, time stamps will be automatically added to each row of data
         * as an integer value rounded down to the unit specified.
         * 
         * @param format The format of timestamp to use. 
         */
        void setTimeStamp(TimeStampFormat format);
         
        /**
         * Defines if data logging should also be streamed over the serial port.
         *
         * @param enable True to enable serial port streaming, false to disable.
         */
        void setSerialMirroring(bool enable);


        /**
         * Creates a new row in the log, ready to be populated by logData()
         * 
         * @return DEVICE_OK on success.
         */
        int beginRow();

        /**
         * Populates the current row with the given key/value pair.
         * @param key the name of the key column) to set.
         * @param value the value to insert 
         * 
         * @return DEVICE_OK on success.
         */
        int logData(const char *key, const char *value);

        /**
         * Populates the current row with the given key/value pair.
         * @param key the name of the key column) to set.
         * @param value the value to insert 
         *
         * @return DEVICE_OK on success.
         */
        int logData(ManagedString key, ManagedString value);

        /**
         * Complete a row in the log, and pushes to persistent storage.
         * @return DEVICE_OK on success.
         */
        int endRow();

        /**
         * Inject the given row into the log as text, ignoring key/value pairs.
         * @param s the string to inject.
         */
        int logString(const char *s);

        /**
         * Inject the given row into the log as text, ignoring key/value pairs.
         * @param s the string to inject.
         */
        int logString(ManagedString s);
        
        /**
         * Get the length of the recorded data
         * @param format the data format
         *          DataFormat::HTMLHeader = 0,   - The HTML header without the data
         *          DataFormat::HTML = 1,               - The entire HTML file
         *          DataFormat::CSV = 2                   - CSV data
         * @return the length of the recorded data
         */
        uint32_t getDataLength(DataFormat format);
        
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
        int readData(void *data, uint32_t index, uint32_t len, DataFormat format, uint32_t length);

        /**
        * Get the number of rows (including the header) that exist in the datalog.
        * @param fromRowIndex 0-based index of starting row
        * @return number of rows + header from fromRowIndex
        */
        uint32_t getNumberOfRows(uint32_t fromRowIndex = 0);

        /**
        * Get n rows (first row is header) worth of logged data as a ManagedString
        * @param fromRowIndex 0-based index of starting row
        * @param nRows number of rows to get from fromRowIndex
        * @return ManagedString of all data in these rows, each column separated by a newline
        */
        ManagedString getRows(uint32_t fromRowIndex, uint32_t nRows);

    private:

        /**
         * Attempt to load an exisitng filesystem, or fomrat a new one if not found.
         */
        void init();

        /*
         * Private APIs methods.
         * These methods enable the functionality of the public APIs, but assume mutual exclusion has already been acquired.
         */
        bool _isPresent();
        void _setVisibility(bool visible);
        void _clear(bool fullErase = CONFIG_MICROBIT_LOG_FULL_ERASE_BY_DEFAULT);
        void _invalidate();
        void _setTimeStamp(TimeStampFormat format);
        int _beginRow();
        int _endRow();
        int _logData(ManagedString key, ManagedString value);
        int _logString(const char *s);
        int _logString(ManagedString s);

        int _readData(uint8_t *data, uint32_t index, uint32_t len, DataFormat format, uint32_t length);
        
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
        int _readSource( uint8_t *&data, uint32_t &index, uint32_t &len, uint32_t &srcIndex, const void *srcPtr, uint32_t srcAddress, uint32_t srcLen);

        /**
         * Add the given heading to the list of headings in use. If the heading already exists,
         * this method has no effect.
         * 
         * @param key the heading to add
         * @param value the initial value to add, or ManagedString::EmptyString
         * @param head true to add the given field at the front of the list, false to add at the end.
         */
        void addHeading(ManagedString key, ManagedString value, bool head = false);

        /**
         * Clean the given buffer of invalid LogFS symbols ("-->" and optionally ",\t\n")
         *
         * @param s the data to clean
         * @param len the number of characters to clean
         * @param removeSeperators if set to false, only "-->" symbols are erased, otherwise ",\t\n" characters are also removed.
         * @return a cleaned version of the string supplied, if any changes are necessary. Otherwise, an empty string is returned.
         */
        ManagedString cleanBuffer(const char *s, int len, bool removeSeparators = true);
    };
}

#endif
