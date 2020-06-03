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

#ifndef MICROBIT_EVENT_SERVICE_H
#define MICROBIT_EVENT_SERVICE_H

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitBLEManager.h"
#include "MicroBitBLEService.h"
#include "MicroBitEvent.h"
#include "EventModel.h"


struct EventServiceEvent
{
    uint16_t    type;
    uint16_t    reason;
};


/**
  * Class definition for a MicroBit BLE Event Service.
  * Provides a BLE gateway onto an Event Model.
  */
class MicroBitEventService : public MicroBitBLEService, MicroBitComponent
{
    public:

    /**
      * Constructor.
      * Create a representation of the EventService
      * @param _ble The instance of a BLE device that we're running on.
      * @param _messageBus An instance of an EventModel which events will be mirrored from.
      */
    MicroBitEventService(BLEDevice &_ble, EventModel &_messageBus);

    /**
     * Periodic callback from MicroBit scheduler.
     * If we're no longer connected, remove any registered Message Bus listeners.
     */
    virtual void idleCallback();

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten( const microbit_ble_evt_write_t *params);

    /**
      * Callback. Invoked when any events are sent on the microBit message bus.
      */
    void onMicroBitEvent(MicroBitEvent evt);

    /**
      * Read callback on microBitRequirements characteristic.
      * Set  params->data and params->length to update the value
      *
      * Used to iterate through the events that the code on this micro:bit is interested in.
      */
    void onDataRead( microbit_onDataRead_t *params);

    private:

    // messageBus we're using.
	EventModel	        &messageBus;

    // memory for our event characteristics.
    EventServiceEvent   clientEventBuffer;
    EventServiceEvent   microBitEventBuffer;
    EventServiceEvent   microBitRequirementsBuffer;
    EventServiceEvent   clientRequirementsBuffer;

    // Message bus offset last sent to the client...
    uint16_t messageBusListenerOffset;
    
    // Index for each charactersitic in arrays of handles and UUIDs
    typedef enum mbbs_cIdx
    {
        mbbs_cIdxMEVENT,
        mbbs_cIdxMREQ,
        mbbs_cIdxCEVENT,
        mbbs_cIdxCREQ,
        mbbs_cIdxCOUNT
    } mbbs_cIdx;
    
    // UUIDs for our service and characteristics
    static const uint8_t  base_uuid[16];
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
