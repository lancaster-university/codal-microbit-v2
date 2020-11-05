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

#include "MicroBitCompat.h"

void 
codal::fiber_add_idle_component(codal::CodalComponent *c)
{
    c->status |= DEVICE_COMPONENT_STATUS_IDLE_TICK;
}

uint32_t codal::htonl(uint32_t v)
{
    uint32_t result;
    uint8_t *w = (uint8_t *) &result;
    uint8_t *r = (uint8_t *) &v;

    r += 3;

    for (int i=0; i<4; i++)
        *w++ = *r--;

    return result;
}

uint16_t codal::htons(uint16_t v)
{
    uint16_t result;
    uint8_t *w = (uint8_t *) &result;
    uint8_t *r = (uint8_t *) &v;

    r += 1;

    for (int i=0; i<2; i++)
        *w++ = *r--;

    return result;
}