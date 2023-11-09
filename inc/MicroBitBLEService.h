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

#ifndef MICROBIT_BLE_SERVICE_H
#define MICROBIT_BLE_SERVICE_H

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitBLEChar.h"

/**
  * Class definition for MicroBitBLEService.
  * Provides a base class for the BLE sevices.
  */
class MicroBitBLEService
{
    public:

    typedef struct microbit_onDataRead_t
    {
        microbit_charhandle_t   handle;
        uint16_t                offset;
        bool                    allow;
        bool                    update;
        uint8_t const          *data;
        uint16_t                length;
    } microbit_onDataRead_t;
        
    /**
      * Constructor.
      * Create a representation of the LEDService
      * @param _ble The instance of a BLE device that we're running on.
      * @param _display An instance of MicroBitDisplay to interface with.
      */
    MicroBitBLEService();

    ~MicroBitBLEService();

    ble_uuid_t getServiceUUID();

    protected:
    
    void RegisterBaseUUID( const uint8_t *bytes16UUID);

    void CreateService( uint16_t uuid);
    
    void CreateCharacteristic(
        int         idx,
        uint16_t    uuid,
        uint8_t    *value,
        uint16_t    init_len,
        uint16_t    max_len,
        uint16_t    props);

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    virtual void onDataWritten( const microbit_ble_evt_write_t *params);
                          
    /**
      * Callback. Invoked when any of our attributes are read via BLE.
      * Set  params->data and params->length to update the value
      */
    virtual void onDataRead( microbit_onDataRead_t *params);
    
    /**
      * Callback. Invoked when an indication is confirmed.
      * Set  params->data and params->length to update the value
      */
    virtual void onConfirmation( const microbit_ble_evt_hvc_t *params);
    
    public:
    
    microbit_gaphandle_t getConnectionHandle();
    
    bool getConnected();
    
    bool setChrValue( int idx, const uint8_t *data, uint16_t length)
    { return characteristicPtr( idx)->setChrValue( getConnectionHandle(), data, length); }

    bool notifyChrValue( int idx, const uint8_t *data, uint16_t length)
    { return characteristicPtr( idx)->notifyChrValue( getConnectionHandle(), data, length); }

    bool indicateChrValue( int idx, const uint8_t *data, uint16_t len)
    { return characteristicPtr( idx)->indicateChrValue( getConnectionHandle(), data, len); }
                                             
    bool writeChrValue( int idx, const uint8_t *data, uint16_t len)
    { return characteristicPtr( idx)->writeChrValue( getConnectionHandle(), data, len); }
    
    bool notifyChrValueEnabled( int idx)
    { return characteristicPtr( idx)->cccdNotify(); }
    
    bool indicateChrValueEnabled( int idx)
    { return characteristicPtr( idx)->cccdIndicate(); }

    int charHandleToIdx( uint16_t handle, microbit_charattr_t *type);
    
    microbit_charhandles_t *charHandles( int idx) { return characteristicPtr( idx)->charHandles(); }
    microbit_charhandle_t   valueHandle( int idx) { return characteristicPtr( idx)->charHandles()->value; }

    virtual int              characteristicCount()          = 0;
    virtual MicroBitBLEChar *characteristicPtr( int idx)    = 0;

    virtual bool onBleEvent(            const microbit_ble_evt_t *p_ble_evt);
    virtual void onConnect(             const microbit_ble_evt_t *p_ble_evt);
    virtual void onDisconnect(          const microbit_ble_evt_t *p_ble_evt);
    virtual void onWrite(               const microbit_ble_evt_t *p_ble_evt);
    virtual void onAuthorizeRequest(    const microbit_ble_evt_t *p_ble_evt);
    virtual void onAuthorizeRead(       const microbit_ble_evt_t *p_ble_evt);
    virtual void onAuthorizeWrite(      const microbit_ble_evt_t *p_ble_evt);
    virtual void onHVC(                 const microbit_ble_evt_t *p_ble_evt);

    protected:

    uint8_t                     bs_uuid_type;
    microbit_servicehandle_t    bs_service_handle;
    ble_uuid_t                  bs_service_uuid;

    static const uint8_t        bs_base_uuid[16];
};


#endif
#endif
