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

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef NRF52_DMA_LED_MATRIX_H
#define NRF52_DMA_LED_MATRIX_H

#include "CodalConfig.h"
#include "Display.h"
#include "MatrixMap.h"
#include "NRFLowLevelTimer.h"

namespace codal
{
    /**
      * Class definition for NRF52DmaLEDMatrix.
      *
      * A LEDMatrix represents the LED matrix array on the micro:bit.
      */
    class NRF52DmaLEDMatrix : public Display
    {
        const MatrixMap &matrixMap;
        uint32_t dmaBuffer[9];
        uint8_t enabled;
        uint8_t strobeRow;
        uint8_t lightLevel;
        DisplayMode
        mode;
        DisplayRotation rotation;

        public:

        /**
          * Constructor.
          *
          * Create a software representation an LED matrix.
          * The display is initially blank.
          *
          * @param map The mapping information that relates pin inputs/outputs to physical screen coordinates.
          * @param id The id the display should use when sending events on the MessageBus. Defaults to DEVICE_ID_DISPLAY.
          * @param mode The DisplayMode to use. Default: DISPLAY_MODE_BLACK_AND_WHITE.
          */
        NRF52DmaLEDMatrix(const MatrixMap &map, uint16_t id = DEVICE_ID_DISPLAY, DisplayMode mode = DISPLAY_MODE_BLACK_AND_WHITE);

        /**
         * Configures the mode of the display.
         *
         * @param mode The mode to swap the display into. One of: DISPLAY_MODE_GREYSCALE,
         *             DISPLAY_MODE_BLACK_AND_WHITE, DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE
         */
        virtual void setDisplayMode(DisplayMode mode);

        /**
         * Retrieves the mode of the display.
         *
         * @return the current mode of the display
         */
        virtual DisplayMode getDisplayMode();

        /**
          * Rotates the display to the given position.
          *
          * Axis aligned values only.
          */
        virtual void rotateTo(DisplayRotation rotation);

        /**
          * Enables the display, should only be called if the display is disabled.
          *
          * @note Only enables the display if the display is currently disabled.
          */
        virtual void enable();

        /**
          * Disables the display, which releases control of the GPIO pins used by the display,
          * which are exposed on the edge connector.
          */
        virtual void disable();

        /**
          * Clears the display of any remaining pixels.
          *
          * `display.image.clear()` can also be used!
          */
        virtual void clear();

        /**
         * Configures the brightness of the display.
         *
         * @param b The brightness to set the brightness to, in the range 0 - 255.
         *
         * @return DEVICE_OK, or DEVICE_INVALID_PARAMETER
         */
        virtual int setBrightness(int b);

        /**
         * Determines the last ambient light level sensed.
         *
         * @return The light level sensed, as an unsigned 8-bit value in the range 0..255
         */
        virtual int readLightLevel();

        /**
         * Puts the component in (or out of) sleep (low power) mode.
         */
        virtual int setSleep(bool doSleep);

        /**
          * Destructor for LEDMatrix, where we deregister this instance from the array of system components.
          */
        ~NRF52DmaLEDMatrix();

        /**
         * A periodic callback invoked by the fiber scheduler in holding fibres.
         * Used to render the display.
         */
        virtual void systemTick();
    };
}

#endif