/*
The MIT License (MIT)

Copyright (c) 2016 Lancaster University, UK.

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
#include "MicroBitDevice.h"

#if CONFIG_ENABLED(MICROBIT_BLE_ENABLED)
#include "nrf_sdm.h"
#endif

using namespace codal;

// internal reference to any created instance of the class - used purely for convenience functions.
MicroBitDevice *microbit_device_instance = NULL;

/**
  * Constructor.
  */
MicroBitDevice::MicroBitDevice()
{
    microbit_device_instance = this;
}

namespace codal {

int microbit_random(int max)
{
    return microbit_device_instance ? microbit_device_instance->random(max) : 0;
}

void microbit_panic(int statusCode)
{
    target_panic(statusCode);
}

void microbit_panic_timeout(int iterations)
{
    //TODO: Populate this once a graphical panic is implemented
}

/**
 * Derive a unique, consistent serial number of this device from internal data.
 *
 * @return the serial number of this device.
 */
uint32_t microbit_serial_number()
{
    return NRF_FICR->DEVICEID[1];
}

/**
  * Determines if a BLE stack is currently running.
  *
  * @return true is a bluetooth stack is operational, false otherwise.
  */
bool ble_running()
{
    uint8_t t = 0;

#if CONFIG_ENABLED(MICROBIT_BLE_ENABLED) 
    sd_softdevice_is_enabled(&t);
#endif

    return t==1;
}

/**
  * Perform a hard reset of the micro:bit.
  */
void
microbit_reset()
{
    NVIC_SystemReset();
}

}
