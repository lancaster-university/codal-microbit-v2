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
#include "MicroBitBLEService.h"

#include "ble.h"
#include "ble_srv_common.h"
#include "ble_conn_state.h"
#include "peer_manager.h"


const uint8_t MicroBitBLEService::bs_base_uuid[ 16] =
{ 0xe9,0x5d,0x00,0x00,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8 };

/**
  * Constructor.
  * Create a representation of a BLEService
  * @param _ble An instance of MicroBitBLEManager.
  */
MicroBitBLEService::MicroBitBLEService() :
    bs_uuid_type(0),
    bs_service_handle(0)
{
    MicroBitBLEServices::getShared()->AddService( this);
}


MicroBitBLEService::~MicroBitBLEService()
{
    MicroBitBLEServices::getShared()->RemoveService( this);
}


void MicroBitBLEService::RegisterBaseUUID( const uint8_t *bytes16UUID)
{
    ble_uuid128_t uuid128;
    for ( int i = 0; i < 16; i++)
        uuid128.uuid128[i] = bytes16UUID[ 15 - i];
    
    MICROBIT_BLE_ECHK( sd_ble_uuid_vs_add( &uuid128, &bs_uuid_type));
    
    MICROBIT_DEBUG_DMESG( "MicroBitBLEService::RegisterBaseUUID bs_uuid_type = %d", (int) bs_uuid_type);
}


void MicroBitBLEService::CreateService( uint16_t uuid)
{
    ble_uuid_t serviceUUID;
    
    serviceUUID.uuid = uuid;
    serviceUUID.type = bs_uuid_type;

    MICROBIT_BLE_ECHK( sd_ble_gatts_service_add( BLE_GATTS_SRVC_TYPE_PRIMARY, &serviceUUID, &bs_service_handle));
    
    MICROBIT_DEBUG_DMESG( "MicroBitBLEService::CreateService( %x) = %d", (unsigned int) uuid, (int) bs_service_handle);
}

                      
void MicroBitBLEService::CreateCharacteristic(
    int             idx,
    uint16_t        uuid,
    uint8_t        *value,
    uint16_t        init_len,
    uint16_t        max_len,
    uint16_t        props)
{
    ble_add_char_params_t params;
    memset( &params, 0, sizeof( params));
    
    params.uuid                 = uuid;
    params.uuid_type            = bs_uuid_type;
    params.max_len              = max_len;
    params.init_len             = init_len;
    params.p_init_value         = value;
    params.is_var_len           = max_len != init_len;

    if ( props & microbit_propREAD)            params.char_props.read              = 1;
    if ( props & microbit_propWRITE_WITHOUT)   params.char_props.write_wo_resp     = 1;
    if ( props & microbit_propWRITE)           params.char_props.write             = 1;
    if ( props & microbit_propNOTIFY)          params.char_props.notify            = 1;
    if ( props & microbit_propINDICATE)        params.char_props.indicate          = 1;
    if ( props & microbit_propSIGNED_WRITES)   params.char_props.auth_signed_wr    = 1;
    if ( props & microbit_propBROADCAST)       params.char_props.broadcast         = 1;

    //ble_gatt_char_ext_props_t   char_ext_props;
    
    if ( props & microbit_propREADAUTH)        params.is_defered_read  = true;
    if ( props & microbit_propWRITEAUTH)       params.is_defered_write = true;

    MICROBIT_DEBUG_DMESG( "MICROBIT_BLE_SECURITY_MODE %d", (int) MICROBIT_BLE_SECURITY_MODE);
          
    params.read_access          = ( security_req_t) MICROBIT_BLE_SECURITY_MODE;
    params.write_access         = ( security_req_t) MICROBIT_BLE_SECURITY_MODE;
    params.cccd_write_access    = ( security_req_t) MICROBIT_BLE_SECURITY_MODE;
    
    params.is_value_user        = true; // All values content stored in the application
    
    //ble_add_char_user_desc_t    *p_user_descr;
    //ble_gatts_char_pf_t         *p_presentation_format;
    
    MICROBIT_BLE_ECHK( characteristic_add( bs_service_handle, &params, ( ble_gatts_char_handles_t *) charHandles( idx)));
    
    MICROBIT_DEBUG_DMESG( "MicroBitBLEService::CreateCharacteristic( %x) = %d %d %d %d",
          (unsigned int) uuid,
          (int) charHandles( idx)->value,
          (int) charHandles( idx)->desc,
          (int) charHandles( idx)->cccd,
          (int) charHandles( idx)->sccd);
}


microbit_gaphandle_t MicroBitBLEService::getConnectionHandle()
{
    ble_conn_state_conn_handle_list_t list = ble_conn_state_periph_handles();
    return list.len ? list.conn_handles[0] : BLE_CONN_HANDLE_INVALID;
}


bool MicroBitBLEService::getConnected()
{
    return ble_conn_state_peripheral_conn_count() > 0;
}

                                            
int MicroBitBLEService::charHandleToIdx( uint16_t handle, microbit_charattr_t *type)
{
    int charCount = characteristicCount();
    
    for ( int idx = 0; idx < charCount; idx++)
    {
        microbit_charhandles_t *p = charHandles( idx);
        
        if ( handle == p->value)    { *type = microbit_charattrVALUE;   return idx; }
        if ( handle == p->desc)     { *type = microbit_charattrDESC;    return idx; }
        if ( handle == p->cccd)     { *type = microbit_charattrCCCD;    return idx; }
        if ( handle == p->sccd)     { *type = microbit_charattrSCCD;    return idx; }
    }
    
    *type = microbit_charattrINVALID;
    return -1;
}
                      
                      
bool MicroBitBLEService::onBleEvent( microbit_ble_evt_t const * p_ble_evt)
{
  //MICROBIT_DEBUG_DMESG( "MicroBitBLEService::onBleEvent %x", (int) p_ble_evt->header.evt_id);
    
  switch ( p_ble_evt->header.evt_id)
  {
      case BLE_GAP_EVT_CONNECTED:
          //TODO: store handle and connected flag?
          onConnect( p_ble_evt);
          break;

      case BLE_GAP_EVT_DISCONNECTED:
          //TODO: clear handle and connected flag?
          onDisconnect( p_ble_evt);

          for ( int idx = 0; idx < characteristicCount(); idx++)
              characteristicPtr( idx)->setCCCD(0);
          break;

      case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
          onAuthorizeRequest( p_ble_evt);
          break;

      case BLE_GATTS_EVT_HVC:
          onHVC( p_ble_evt);
          break;

      case BLE_GATTS_EVT_WRITE:
          onWrite( p_ble_evt);
          break;

      default:
          // no implementation
          break;
  }

  return true;
}


void MicroBitBLEService::onConnect( const microbit_ble_evt_t *p_ble_evt)
{
    MICROBIT_DEBUG_DMESG( "MicroBitBLEService::onConnect");
}


void MicroBitBLEService::onDisconnect( const microbit_ble_evt_t *p_ble_evt)
{
    MICROBIT_DEBUG_DMESG( "MicroBitBLEService::onDisconnect");
}

                      
void MicroBitBLEService::onWrite( const microbit_ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    
    MICROBIT_DEBUG_DMESG( "MicroBitBLEService::onWrite %d", (int) p_evt_write->handle);
    
    microbit_charattr_t type;
    int idx = charHandleToIdx( p_evt_write->handle, &type);
    if ( idx < 0)
        return;

    MICROBIT_DEBUG_DMESG( "onWrite type = %d", (int) type);
    
    if ( type == microbit_charattrCCCD && p_evt_write->len == 2)
        characteristicPtr( idx)->setCCCD( uint16_decode( p_evt_write->data));

    onDataWritten( p_evt_write);
}


/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitBLEService::onDataWritten( const microbit_ble_evt_write_t *params)
{
    
}


void MicroBitBLEService::onAuthorizeRequest( const microbit_ble_evt_t *p_ble_evt)
{
    switch ( p_ble_evt->evt.gatts_evt.params.authorize_request.type)
    {
        case BLE_GATTS_AUTHORIZE_TYPE_INVALID:
            break;
            
        case BLE_GATTS_AUTHORIZE_TYPE_READ:
            onAuthorizeRead( p_ble_evt);
            break;
            
        case BLE_GATTS_AUTHORIZE_TYPE_WRITE:
            onAuthorizeWrite( p_ble_evt);
            break;
    }
}


void MicroBitBLEService::onAuthorizeRead( const microbit_ble_evt_t *p_ble_evt)
{
    const ble_gatts_evt_read_t *req = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;

    microbit_charattr_t type;
    int idx = charHandleToIdx( req->handle, &type);
    if ( idx < 0)
        return;
    
    //DMESG( "MicroBitBLEService::onAuthorizeRead %d %d", (int) req->handle, (int) type);
    
    ble_gatts_rw_authorize_reply_params_t reply;
    reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
    
    ble_gatts_authorize_params_t *auth = &reply.params.read;

    if ( type != microbit_charattrVALUE)
    {
        MICROBIT_DEBUG_DMESG( "MicroBitBLEService::onAuthorizeRead type = %d", (int) type);
        auth->gatt_status = BLE_GATT_STATUS_SUCCESS;
        auth->update      = 0;
        auth->offset      = req->offset;
        auth->len         = 0;
        auth->p_data      = NULL;
        MICROBIT_BLE_ECHK( sd_ble_gatts_rw_authorize_reply( p_ble_evt->evt.gatts_evt.conn_handle, &reply));
        return;
    }
    
    microbit_onDataRead_t params;
    params.handle = req->handle;
    params.offset = req->offset;
    params.allow  = true;
    params.update = true;
    params.data   = NULL;
    params.length = 0;

    onDataRead( &params);
    
    auth->gatt_status = params.allow ? BLE_GATT_STATUS_SUCCESS : BLE_GATT_STATUS_ATTERR_READ_NOT_PERMITTED;
    auth->update      = params.update;
    auth->offset      = params.offset;
    auth->len         = params.length;
    auth->p_data      = params.data;

    //MICROBIT_DEBUG_DMESG( "MicroBitBLEService::onAuthorizeRead status = %d data = %x len = %d", (int) auth->gatt_status, (unsigned int) auth->p_data, (int) auth->len);
    
    MICROBIT_BLE_ECHK( sd_ble_gatts_rw_authorize_reply( p_ble_evt->evt.gatts_evt.conn_handle, &reply));
}
    
    
/**
  * Callback. Invoked when any of our attributes are read via BLE.
  * Set  params->data and params->length to update the value
  */
void MicroBitBLEService::onDataRead( microbit_onDataRead_t *params)
{
}


void MicroBitBLEService::onAuthorizeWrite( const microbit_ble_evt_t *p_ble_evt)
{
    const ble_gatts_evt_write_t *req = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;

    microbit_charattr_t type;
    int idx = charHandleToIdx( req->handle, &type);
    if ( idx < 0)
        return;

    //MICROBIT_DEBUG_DMESG( "MicroBitBLEService::onAuthorizeWrite handle %d type %d", (int) req->handle, (int) type);
}

    
void MicroBitBLEService::onHVC( const microbit_ble_evt_t *p_ble_evt)
{
    //MICROBIT_DEBUG_DMESG( "MicroBitBLEService::onHVC");
    onConfirmation( &p_ble_evt->evt.gatts_evt.params.hvc);
}

void MicroBitBLEService::onConfirmation( const microbit_ble_evt_hvc_t *params)
{
}

#endif
