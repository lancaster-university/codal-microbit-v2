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

#include "CodalDmesg.h"

// MICROBIT_DMESG_LEVEL : 0 off; 1 error; 2 warning; 3 info; 4 debug

#if (DEVICE_DMESG_BUFFER_SIZE > 0)
#ifndef MICROBIT_DMESG_LEVEL
#define MICROBIT_DMESG_LEVEL 1
#endif
#else
#ifdef MICROBIT_DMESG_LEVEL
#undef MICROBIT_DMESG_LEVEL
#endif
#define MICROBIT_DMESG_LEVEL 0
#endif

#if ( MICROBIT_DMESG_LEVEL > 3)
#define MICROBIT_DEBUG_DMESG  codal_dmesg
#define MICROBIT_DEBUG_DMESGN codal_dmesg_nocrlf
#define MICROBIT_DEBUG_DMESGF codal_dmesg_with_flush
#else
#define MICROBIT_DEBUG_DMESG(...)  ((void)0)
#define MICROBIT_DEBUG_DMESGN(...) ((void)0)
#define MICROBIT_DEBUG_DMESGF(...) ((void)0)
#endif

#if ( MICROBIT_DMESG_LEVEL > 2)
#define MICROBIT_INFO_DMESG  codal_dmesg
#define MICROBIT_INFO_DMESGN codal_dmesg_nocrlf
#define MICROBIT_INFO_DMESGF codal_dmesg_with_flush
#else
#define MICROBIT_INFO_DMESG(...)  ((void)0)
#define MICROBIT_INFO_DMESGN(...) ((void)0)
#define MICROBIT_INFO_DMESGF(...) ((void)0)
#endif

#if ( MICROBIT_DMESG_LEVEL > 1)
#define MICROBIT_WARNING_DMESG  codal_dmesg
#define MICROBIT_WARNING_DMESGN codal_dmesg_nocrlf
#define MICROBIT_WARNING_DMESGF codal_dmesg_with_flush
#else
#define MICROBIT_WARNING_DMESG(...)  ((void)0)
#define MICROBIT_WARNING_DMESGN(...) ((void)0)
#define MICROBIT_WARNING_DMESGF(...) ((void)0)
#endif

#if ( MICROBIT_DMESG_LEVEL > 0)
#define MICROBIT_ERROR_DMESG  codal_dmesg
#define MICROBIT_ERROR_DMESGN codal_dmesg_nocrlf
#define MICROBIT_ERROR_DMESGF codal_dmesg_with_flush
#else
#define MICROBIT_ERROR_DMESG(...)  ((void)0)
#define MICROBIT_ERROR_DMESGN(...) ((void)0)
#define MICROBIT_ERROR_DMESGF(...) ((void)0)
#endif


#if CONFIG_ENABLED(DEVICE_BLE)

#include "ble.h"
#include "ble_gatts.h"

typedef uint32_t                     microbit_ble_ret_code_t;
typedef uint16_t                     microbit_gaphandle_t;
typedef uint16_t                     microbit_servicehandle_t;
typedef uint16_t                     microbit_charhandle_t;
typedef ble_evt_t                    microbit_ble_evt_t;
typedef ble_gatts_evt_write_t        microbit_ble_evt_write_t;
typedef ble_gatts_evt_hvc_t          microbit_ble_evt_hvc_t;

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


#if ( MICROBIT_DMESG_LEVEL > 0)
extern microbit_ble_ret_code_t microbit_ble_on_error( microbit_ble_ret_code_t err, const char *msg);
#define MICROBIT_BLE_ECHK( err) microbit_ble_on_error( err, #err)
#else
#define MICROBIT_BLE_ECHK( err) (err)
#endif

#endif

#endif
