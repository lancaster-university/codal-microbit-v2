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

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitBLETypes.h"

#include "nrf_log_ctrl.h"

using namespace codal;

#if ( MICROBIT_DMESG_LEVEL >= MICROBIT_DMESG_LEVEL_DEBUG)
microbit_ble_ret_code_t microbit_ble_on_error( microbit_ble_ret_code_t err, const char *msg)
{
    NRF_LOG_FLUSH();
    if ( err != NRF_SUCCESS)
    {
        MICROBIT_DEBUG_DMESGN( "ERROR %x from ", (int)err);
        MICROBIT_DEBUG_DMESG( msg);
    }
    return err;
}
#endif

#endif // CONFIG_ENABLED(DEVICE_BLE)
