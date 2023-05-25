/*
The MIT License (MIT)

This class has been written for the Microbit Educational Foundation.

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

#ifndef MICROBIT_UTILITY_SERVICE_H
#define MICROBIT_UTILITY_SERVICE_H

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
#include "MicroBitLog.h"


class MicroBitUtilityWorkspace;


/**
 * Class declration for the custom MicroBit Utility Service.
 * Provides simple BLE request/response
 * See MicroBitUtilityTypes.h
 */
class MicroBitUtilityService : public MicroBitBLEService
{
    static MicroBitUtilityService *shared;
    
    public:
    /**
     * Return a pointer to the shared service
     * @return a pointer to the service, or NULL if it has not been created
     */
    static MicroBitUtilityService *getShared()
    {
        return shared;
    }

    /**
     * Return a pointer to the shared service, creating it if necessary
     * @param _ble an instance of BLEDevice
     * @param _messageBus An instance of a MessageBus to interface with.
     * @param _storage A persistent storage manager to use to hold non-volatile state.
     * @param _log a log storage manager to read stored data.
     * @return a pointer to the service, or NULL if it has not been created
     */
    static MicroBitUtilityService *createShared( BLEDevice &_ble, EventModel &_messageBus, MicroBitStorage &_storage, MicroBitLog &_log);

    public:
    /**
     * Constructor.
     * @param _ble an instance of BLEDevice
     * @param _messageBus An instance of a MessageBus to interface with.
     * @param _storage A persistent storage manager to use to hold non-volatile state.
     * @param _log a log storage manager to read stored data.
     */
    MicroBitUtilityService( BLEDevice &_ble, EventModel &_messageBus, MicroBitStorage &_storage, MicroBitLog &_log);

    private:
    /**
     * Invoked when BLE connects.
     * Start listening for events
    */
    void onConnect( const microbit_ble_evt_t *p_ble_evt);

    /**
     * Invoked when BLE disconnects.
     * Stop listening for events
    */
    void onDisconnect( const microbit_ble_evt_t *p_ble_evt);

    /**
     * Callback. Invoked when any of our attributes are written via BLE.
     * Record the  request, and trigger processing.
     * Requests are not queued. If a request is actively being processed, ignore the new request, otherwise simply override the previous one.
     */
    void onDataWritten(const microbit_ble_evt_write_t *params);

    /**
     * Callback. Invoked when a registered event occurs.
     */
    void onEvent(MicroBitEvent e);

    private:
    // MessageBus we're using
    EventModel          &messageBus;
    MicroBitStorage     &storage;
    MicroBitLog         &log;

    uint8_t characteristicValue[ 20];

    // Index for each charactersitic in arrays of handles and UUIDs
    typedef enum mbbs_cIdx
    {
        mbbs_cIdxCTRL,
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
    
    private:
    
    MicroBitUtilityWorkspace *workspace;
    
    /**
     * Listen for or ignore  events for processing requests
     * @param yes true to listen, otherwise ignore
     */
    void listen( bool yes);
    
    /**
     * Send a reply packet
     * @param data Data to send
     * @param length Length if the data
     * @return DEVICE_OK if sent
     */
    int sendReply( const void *data, uint16_t length);
    
    /**
     * Process the current request. This may block.
     * @return DEVICE_OK if finished
     */
    int processRequest();

    /**
     * Process request typeLogLength
     * @return DEVICE_OK if finished
     */
    int processLogLength();

    /**
     * Process request typeLogRead
     * @return DEVICE_OK if finished
     */
    int processLogRead();
};


#endif
#endif
