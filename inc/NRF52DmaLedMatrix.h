/*
The MIT License (MIT)

Copyright (c) 2021 Lancaster University.

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

#ifndef NRF52_DMA_LED_MATRIX_H
#define NRF52_DMA_LED_MATRIX_H

#include "codal-core/inc/core/CodalConfig.h"
#include "codal-core/inc/driver-models/Display.h"
#include "codal-core/inc/driver-models/MatrixMap.h"
#include "codal-nrf52/inc/NRFLowLevelTimer.h"

namespace codal
{

/**
 * A low-cpu usage driver for the micro:bit v2's 5x5 LED matrix.
 *
 * This driver uses DMA, PWM, and PPI to refresh the display automatically,
 * which significantly reduces the CPU load compared to the interrupt-driven
 * approach.
 */
class NRF52DmaLEDMatrix : public Display
{
public:
    /**
     * Constructor.
     *
     * @param map The mapping information for the LED matrix.
     * @param timer The timer to use for display refresh.
     * @param id The id of this component.
     */
    NRF52DmaLEDMatrix(const MatrixMap &map, NRFLowLevelTimer &timer, uint16_t id = DEVICE_ID_DISPLAY);

    /**
     * Configures the display for the given frame.
     */
    virtual void render() override;

    /**
     * Rotates the display to the given position.
     */
    virtual void rotateTo(DisplayRotation rotation) override;

    /**
     * Enables the display.
     */
    virtual void enable() override;

    /**
     * Disables the display.
     */
    virtual void disable() override;

    /**
     * Sets the brightness of the display.
     */
    virtual int setBrightness(int b) override;

private:
    const MatrixMap &matrixMap;
    NRFLowLevelTimer &timer;
    uint16_t pwm_buffer[5][5];
    bool enabled;
};

}

#endif