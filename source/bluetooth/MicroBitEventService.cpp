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
  * Class definition for a MicroBit BLE Event Service.
  * Provides a BLE gateway onto an Event Model.
  */

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitEventService.h"
#include "ExternalEvents.h"
#include "MicroBitFiber.h"


const uint16_t MicroBitEventService::serviceUUID               = 0x93af;
const uint16_t MicroBitEventService::charUUID[ mbbs_cIdxCOUNT] = { 0x9775, 0xb84c, 0x5404, 0x23c4 };


/**
  * Constructor.
  * Create a representation of the EventService
  * @param _ble The instance of a BLE device that we're running on.
  * @param _messageBus An instance of an EventModel which events will be mirrored from.
  */
MicroBitEventService::MicroBitEventService(BLEDevice &_ble, EventModel &_messageBus) :
        messageBus(_messageBus)
{
    // Initialise our characteristic values.
    clientEventBuffer.type = 0x00;
    clientEventBuffer.reason = 0x00;
    microBitEventBuffer = microBitRequirementsBuffer = clientRequirementsBuffer = clientEventBuffer;
    
    messageBusListenerOffset = 0;

    // Register the base UUID and create the service.
    RegisterBaseUUID( bs_base_uuid);
    CreateService( serviceUUID);

    CreateCharacteristic( mbbs_cIdxMEVENT, charUUID[ mbbs_cIdxMEVENT],
                        (uint8_t *)&microBitEventBuffer,
                         sizeof(EventServiceEvent), sizeof(EventServiceEvent),
                         microbit_propREAD | microbit_propNOTIFY);

    CreateCharacteristic( mbbs_cIdxCEVENT, charUUID[ mbbs_cIdxCEVENT],
                         (uint8_t *)&clientEventBuffer,
                         sizeof(EventServiceEvent), sizeof(EventServiceEvent),
                         microbit_propWRITE | microbit_propWRITE_WITHOUT);

    CreateCharacteristic( mbbs_cIdxCREQ, charUUID[ mbbs_cIdxCREQ],
                         (uint8_t *)&clientRequirementsBuffer,
                         sizeof(EventServiceEvent), sizeof(EventServiceEvent),
                         microbit_propWRITE);

    CreateCharacteristic( mbbs_cIdxMREQ, charUUID[ mbbs_cIdxMREQ],
                         (uint8_t *)&microBitRequirementsBuffer,
                         sizeof(EventServiceEvent),
                         sizeof(EventServiceEvent),
                         microbit_propREAD | microbit_propNOTIFY | microbit_propREADAUTH);

    fiber_add_idle_component(this);
}


/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitEventService::onDataWritten( const microbit_ble_evt_write_t *params)
{
    int len = params->len;
    EventServiceEvent *e = (EventServiceEvent *)params->data;

    if ( params->handle == valueHandle( mbbs_cIdxCEVENT))
    {
        // Read and fire all events...
        while (len >= 4)
        {
            MicroBitEvent evt(e->type, e->reason);
            len-=4;
            e++;
        }
        return;
    }

    if ( params->handle == valueHandle( mbbs_cIdxCREQ))
    {
        // Read and register for all the events given...
        while (len >= 4)
        {
            // Using MESSAGE_BUS_LISTENER_IMMEDIATE results in a crash in notify()
            messageBus.listen(e->type, e->reason, this, &MicroBitEventService::onMicroBitEvent);

            len-=4;
            e++;
        }
        return;
    }
}

/**
  * Callback. Invoked when any events are sent on the microBit message bus.
  */
void MicroBitEventService::onMicroBitEvent(MicroBitEvent evt)
{
    EventServiceEvent *e = &microBitEventBuffer;

    if ( getConnected())
    {
        e->type = evt.source;
        e->reason = evt.value;
        notifyChrValue( mbbs_cIdxMEVENT, (const uint8_t *)e, sizeof(EventServiceEvent));
    }
}

/**
  * Periodic callback from MicroBit scheduler.
  * If we're no longer connected, remove any registered Message Bus listeners.
  */
void MicroBitEventService::idleCallback()
{
    if ( !getConnected() && messageBusListenerOffset > 0)
    {
        messageBusListenerOffset = 0;
        messageBus.ignore(MICROBIT_ID_ANY, MICROBIT_EVT_ANY, this, &MicroBitEventService::onMicroBitEvent);
    }
}

/**
  * Read callback on microBitRequirements characteristic.
  * Set  params->data and params->length to update the value
  *
  * Used to iterate through the events that the code on this micro:bit is interested in.
  */
void MicroBitEventService::onDataRead( microbit_onDataRead_t *params)
{
    if ( params->handle == valueHandle( mbbs_cIdxMREQ))
    {
        // Walk through the list of message bus listeners.
        // We send one at a time, and our client can keep reading from this characterisitic until we return an emtpy value.
        MicroBitListener *l = messageBus.elementAt( messageBusListenerOffset++);

        if (l != NULL)
        {
            microBitRequirementsBuffer.type = l->id;
            microBitRequirementsBuffer.reason = l->value;
            params->data = (uint8_t *)&microBitRequirementsBuffer;
            params->length = sizeof(EventServiceEvent);
        }
        else
        {
            params->data = (uint8_t *)&microBitRequirementsBuffer;
            params->length = 0;
        }
    }
}

#endif
