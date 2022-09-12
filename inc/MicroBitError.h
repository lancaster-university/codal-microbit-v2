/*
The MIT License (MIT)
Copyright (c) 2022 Lancaster University.
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

#ifndef MicroBit_Error_H
#define MicroBit_Error_H

#include "ErrorNo.h"

// Defined Micro:bit v2.xx specific error codes
// For on-board peripherals these should be from PanicCode::DEVICE_PERIPHERAL_ERROR offset
// to ensure we're in the correct range.
// Note: the maximum offset for this range is +9, to avoid hitting the next block!
enum MicroBitPanic {
    // Accelerometer unable proceed (hardware i/o failure)
    ACCELEROMETER_ERROR = PanicCode::DEVICE_PERIPHERAL_ERROR + 0, // Aka. Panic 50

    // Compass unable to proceed (hardware i/o failure)
    COMPASS_ERROR = PanicCode::DEVICE_PERIPHERAL_ERROR + 1,       // Aka. Panic 51

    // DAPLink comms failure (likely used a daplink feature without a daplink chip, or similar failure)
    USB_INTERFACE_ERROR = PanicCode::DEVICE_PERIPHERAL_ERROR + 2  // Aka. Panic 52
};

#endif
