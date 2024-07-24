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

#ifndef MICROBIT_BLE_SERVICES_H
#define MICROBIT_BLE_SERVICES_H

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitBLEService.h"


#ifndef MICROBIT_BLE_SERVICES_MAX
#define MICROBIT_BLE_SERVICES_MAX 20
#endif

#ifndef MICROBIT_BLE_SERVICES_OBSERVER_PRIO
#define MICROBIT_BLE_SERVICES_OBSERVER_PRIO 2
#endif

namespace codal
{

/**
  * Class definition for MicroBitBLEServices.
  */
class MicroBitBLEServices
{
    public:
    
    /**
     * getShared
     *
     * Allows other objects to obtain a pointer to the single instance of this object.
     */
    static MicroBitBLEServices *getShared();

    private:

    /**
      * Constructor.
      */
    MicroBitBLEServices();

    public:

    void AddService(    MicroBitBLEService *service);
    void RemoveService( MicroBitBLEService *service);
    
    void onBleEvent( microbit_ble_evt_t const * p_ble_evt);

    int                  bs_services_count;
    MicroBitBLEService   *bs_services[ MICROBIT_BLE_SERVICES_MAX];
};

} // namespace codal

#endif // CONFIG_ENABLED(DEVICE_BLE)
#endif // MICROBIT_BLE_SERVICES_H
