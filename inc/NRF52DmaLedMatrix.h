/*
The MIT License (MIT)

Copyright (c) 2024 Lancaster University.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES of MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef NRF52_DMA_LED_MATRIX_H
#define NRF52_DMA_LED_MATRIX_H

#include "CodalConfig.h"
#include "LEDMatrix.h"
#include "NRFLowLevelTimer.h"

#define NRF52_DMA_LED_MATRIX_MAXIMUM_COLUMNS 5
#define NRF52_DMA_LED_MATRIX_MAXIMUM_ROWS 5

namespace codal
{
    class NRF52DmaLEDMatrix : public Display
    {
        uint8_t rotation;
        bool enabled;
        DisplayMode mode;
        uint8_t strobeRow;

        const MatrixMap &matrixMap;
        NRFLowLevelTimer &timer;

        uint16_t pwm_buffer[NRF52_DMA_LED_MATRIX_MAXIMUM_COLUMNS];

        public:

        NRF52DmaLEDMatrix(NRFLowLevelTimer &timer, const MatrixMap &map, uint16_t id = DEVICE_ID_DISPLAY, DisplayMode mode = DisplayMode::DISPLAY_MODE_BLACK_AND_WHITE);

        void render();

        virtual void enable();

        virtual void disable();

        void setDisplayMode(DisplayMode mode);

        DisplayMode getDisplayMode();

        void rotateTo(DisplayRotation position);

        void clear();

        int setBrightness(int b);

        int readLightLevel();

        virtual int setSleep(bool doSleep) override;

        ~NRF52DmaLEDMatrix();
    };
}

#endif