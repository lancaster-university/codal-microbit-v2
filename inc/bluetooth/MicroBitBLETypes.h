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

#ifndef MICROBIT_BLE_TYPES_H
#define MICROBIT_BLE_TYPES_H

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "ble.h"
#include "ble_gatts.h"

typedef uint32_t                     microbit_ble_ret_code_t;
typedef uint16_t                     microbit_gaphandle_t;
typedef uint16_t                     microbit_servicehandle_t;
typedef uint16_t                     microbit_charhandle_t;
typedef ble_evt_t                    microbit_ble_evt_t;
typedef ble_gatts_evt_write_t        microbit_ble_evt_write_t;

typedef enum microbit_prop_t
{
    microbit_propREAD           = 1,
    microbit_propWRITE_WITHOUT  = 2,
    microbit_propWRITE          = 4,
    microbit_propNOTIFY         = 8,
    microbit_propINDICATE       = 16,
    microbit_propSIGNED_WRITES  = 32,
    microbit_propBROADCAST      = 64,
    microbit_propEXTENDED       = 128,
    microbit_propREADAUTH       = 0x100,
    microbit_propWRITEAUTH      = 0x200
} microbit_prop_t;

typedef struct microbit_charhandles_t
{
    microbit_charhandle_t value;
    microbit_charhandle_t desc;
    microbit_charhandle_t cccd;
    microbit_charhandle_t sccd;
} microbit_charhandles_t;

typedef enum microbit_charattr_t
{
    microbit_charattrINVALID,
    microbit_charattrVALUE,
    microbit_charattrDESC,
    microbit_charattrCCCD,
    microbit_charattrSCCD
} microbit_charattr_t;

#if NRF_LOG_ENABLE || ( DEVICE_DMESG_BUFFER_SIZE > 0)
extern microbit_ble_ret_code_t microbit_ble_on_error( microbit_ble_ret_code_t err, const char *msg);
#define MICROBIT_BLE_ECHK( err) microbit_ble_on_error( err, #err)
#else
#define MICROBIT_BLE_ECHK( err) (err)
#endif

#endif

#endif
