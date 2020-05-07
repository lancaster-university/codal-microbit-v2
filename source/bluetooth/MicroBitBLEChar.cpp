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

#include "MicroBitBLEServices.h"
#include "MicroBitBLEChar.h"

#include "ble.h"
#include "ble_srv_common.h"
#include "ble_conn_state.h"

/**
  * Constructor.
  * Create a representation of a BLEService
  * @param _ble An instance of MicroBitBLEManager.
  */
MicroBitBLEChar::MicroBitBLEChar() : cccd(0)
{
    memclr( &handles, sizeof( handles));
}


bool MicroBitBLEChar::setChrValue( microbit_gaphandle_t connection, const uint8_t *data, uint16_t length)
{
    ble_gatts_value_t value;
    value.len     = length;
    value.offset  = 0;
    value.p_value = const_cast<uint8_t *>(data);
    return MICROBIT_BLE_ECHK( sd_ble_gatts_value_set( connection, handles.value, &value)) == NRF_SUCCESS;
}


bool MicroBitBLEChar::notifyChrValue( microbit_gaphandle_t connection, const uint8_t *data, uint16_t length)
{
    if ( connection == BLE_CONN_HANDLE_INVALID)
        return false;
    
    DMESGF( "MicroBitBLEChar::notifyChrValue %d", (int) handles.value);
    
    bool set = false;
    
    if ( cccdNotify())
    {
        ble_gatts_hvx_params_t hvx_params;
        hvx_params.handle = handles.value;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &length;
        hvx_params.p_data = data;
        
        DMESGF( "calling sd_ble_gatts_hvx( %d, %x, %d, %d)",
               (int) handles.value, (unsigned int) data, (int) *data, (int) length);
        
        if ( MICROBIT_BLE_ECHK( sd_ble_gatts_hvx( connection, &hvx_params)) == NRF_SUCCESS)
            return true;
        
        if ( *hvx_params.p_len == length)
            set = true;
    }
    
    if ( !set)
        setChrValue( connection, data, length);
    
    return false;
}
       
                             
bool MicroBitBLEChar::indicateChrValue( microbit_gaphandle_t connection, const uint8_t *data, uint16_t length)
{
    if ( connection == BLE_CONN_HANDLE_INVALID)
        return false;
    
    DMESGF( "MicroBitBLEChar::indicateChrValue %d", (int) handles.value);
    
    bool set = false;
    
    if ( cccdIndicate())
    {
        ble_gatts_hvx_params_t hvx_params;
        hvx_params.handle = handles.value;
        hvx_params.type   = BLE_GATT_HVX_INDICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &length;
        hvx_params.p_data = data;
        
        DMESGF( "calling sd_ble_gatts_hvx( %d, %x, %d, %d)",
               (int) handles.value, (unsigned int) data, (int) *data, (int) length);

        if ( MICROBIT_BLE_ECHK( sd_ble_gatts_hvx( connection, &hvx_params)) == NRF_SUCCESS)
            return true;
        
        if ( *hvx_params.p_len != length)
            set = true;
    }
            
    if ( !set)
        setChrValue( connection, data, length);
    
    return false;
}
                                    
                                                          
bool MicroBitBLEChar::writeChrValue( microbit_gaphandle_t connection, const uint8_t *data, uint16_t length)
{
    if ( connection == BLE_CONN_HANDLE_INVALID)
        return false;
 
    DMESGF( "MicroBitBLEChar::writeChrValue %d", (int) handles.value);
    
    bool done = true;
    
    if ( cccdNotify() || cccdIndicate())
    {
        if ( cccdNotify())
        {
            if ( !notifyChrValue( connection, data, length))
                done = false;
        }
        
        if ( cccdIndicate())
        {
            if ( !indicateChrValue( connection, data, length))
                done = false;
        }
    }
    else
    {
        if ( !setChrValue( connection, data, length))
            done = false;
    }
     
    return done;
}

#endif
