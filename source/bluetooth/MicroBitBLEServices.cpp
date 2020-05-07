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
  * Class definition for MicroBitBLEService.
  * Provides a base class for the BLE sevices.
  */

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitDevice.h"
#include "MicroBitBLEService.h"
#include "MicroBitBLEServices.h"

#include "nrf_sdh_ble.h"


/**
 * getShared
 *
 * Allows other objects to easily obtain a pointer to the single instance of this object. By rights the constructor should be made
 * private to properly implement the singleton pattern.
 *
 */
MicroBitBLEServices *MicroBitBLEServices::getShared()
{
    static MicroBitBLEServices *shared = NULL;
    
    if ( !shared)
        shared = new MicroBitBLEServices();

    return shared;
}


/**
  * Constructor.
  * Create a representation of a BLEService
  * @param _ble An instance of MicroBitBLEManager.
  */
MicroBitBLEServices::MicroBitBLEServices() :
    bs_services_count(0)
{
}


void MicroBitBLEServices::AddService( MicroBitBLEService *service)
{
    if ( bs_services_count >= MICROBIT_BLE_SERVICES_MAX)
        microbit_panic( DEVICE_OOM);

    bs_services[ bs_services_count] = service;
    bs_services_count++;
}


void MicroBitBLEServices::RemoveService( MicroBitBLEService *service)
{
    int count;
    
    for ( count = 0; count < bs_services_count; count++)
    {
        if ( bs_services[ count] == service)
            break;
    }
    
    if ( count < bs_services_count)
    {
        for ( int i = count + 1; i < bs_services_count; i++)
        {
            bs_services[ count] = bs_services[ i];
            count++;
        }
        
        bs_services_count = count;
    }
}


void MicroBitBLEServices::onBleEvent( ble_evt_t const * p_ble_evt)
{
    //DMESG("MicroBitBLEServices::onBleEvent 0x%x", (unsigned int) p_ble_evt->header.evt_id);
    
    for ( int i = 0; i < bs_services_count; i++)
    {
        if ( !bs_services[ i]->onBleEvent( p_ble_evt))
            break;
    }
}


static void microbit_ble_services_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context)
{
    MicroBitBLEServices::getShared()->onBleEvent( p_ble_evt);
}

NRF_SDH_BLE_OBSERVER( microbit_ble_services_obs, MICROBIT_BLE_SERVICES_OBSERVER_PRIO, microbit_ble_services_on_ble_evt, NULL);

#endif
