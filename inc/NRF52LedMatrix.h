/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.

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

#ifndef NRF52_LED_MATRIX_H
#define NRF52_LED_MATRIX_H

#include "CodalConfig.h"
#include "LEDMatrix.h"
#include "neopixel.h"
#include "NRFLowLevelTimer.h"

#define NRF52_LED_MATRIX_CLOCK_FREQUENCY        16000000                               // Frequency of underlying hardware clock (must b 1MHz, 2Mhz 4Mhz, 8Mhz or 16MHz)
#define NRF52_LED_MATRIX_FREQUENCY              60                                     // Frequency of the frame update for the display
#define NRF52_LED_MATRIX_MAXIMUM_COLUMNS        5                                      // The maximum number of LEDMatrix columns supported by the hardware.
#define NRF52_LED_MATRIX_LIGHTSENSE_STROBES     4                                      // Multiple of strobe period to use for light sense
#define NRF52_LED_MATRIX_NEOPIXEL_TIMESLOT      (NRF52_LED_MATRIX_MAXIMUM_COLUMNS + 1) // Timeslot index reserved for accessibilty neopixel use

// TODO: Replace this with a resource allocated version
#define NRF52_LEDMATRIX_GPIOTE_CHANNEL_BASE     1
#define NRF52_LEDMATRIX_PPI_CHANNEL_BASE        3

#define NRF52_LEDMATRIX_STATUS_RESET            0x01
#define NRF52_LEDMATRIX_STATUS_LIGHTREADY       0x02

namespace codal
{
    // Generic represetation of a colour.
    // Migrate this to codal-core at some point...
    class Colour
    {
        public:
        uint8_t        red;
        uint8_t        green;
        uint8_t        blue;

        static constexpr Colour RED() { return Colour{255, 0, 0}; }
    };


    /**
     * Class definition for an optimised LEDMatrix driver using nrf52 PPI and GPIOTE hardware.
     */
    class NRF52LEDMatrix : public Display
    {
        uint8_t strobeRow;                      // The current row being displayed.
        uint8_t timeslots;                      // The total number of timeslots used by the driver (includes light sensing).
        DisplayMode mode;                       // The currnet display mode being used.
        bool enabled;                           // Whether or not the display is enabled.
        uint8_t rotation;                       // DisplayRotation

        const MatrixMap     &matrixMap;         // Data structure that maps screen x/y pixels into GPIO pins.
        NRFLowLevelTimer    &timer;             // The timer module used to drive this LEDMatrix.
        uint32_t            timerPeriod;        // The period of the hardware timer.
        uint32_t            quantum;            // The length of time allotted to each brightness level.
        uint32_t            lightLevel;         // Record of the last light level sampled.
        
        int8_t              gpiote[NRF52_LED_MATRIX_MAXIMUM_COLUMNS];            // GPIOTE channels used by output columns.
        int8_t              ppi[NRF52_LED_MATRIX_MAXIMUM_COLUMNS];               // PPI channels used by output columns.

        public:
        bool                accessibilityEnabled = true;
        NRF52Pin            *accessibilityPin = NULL;
        WS2812B             *ws = NULL;                          // WS2812B accessibility interface
        NRF52PWM            *pwm = NULL;                         // Dedicated PWM generator to use for accessibilty WS2812B
        ManagedBuffer       accessibilityBuf;                    // WS2812B buffer used to stream pixel data.
        Colour              accessibilityColour = Colour::RED(); // The colour to use as the base colour for the WS2812B accessibility interface.

        /**
         * Enable accessibility mode
         */
        void enableAccessibility(bool enable = true);

        /**
         * Configures an optional WS2812B driven accesibility display for this NRF52LedMatrix.
         *
         * @param pin A reference to the pin object to drive the display from
         * @param numberOfLeds The total number of LEDs to drive. Defaults to 25
         */
        void setAccessibilityDisplay(NRF52Pin &pin, const uint16_t numberOfLeds = 25);

        /**
         * Sets or clears the pixel at the given location on an attached accessibility display.
         *
         * @param index Zero based index of the pixel to change.
         * @param value 8 bit brightness data for that pixel.
         */
        void setAccessibilityPixel(uint16_t index, uint8_t value);

        /**
         * Define the pixel colour to use for WS2812B attached accessibilty display.
         * Accessibility needs may require colours other than red for usability.
         *
         * @param colour The pixel colour represented as a Colour type
         */
	    void setAccessibilityBaseColour(Colour colour);

        /**
         * Refresh the attached WS2812B accessibility display.
         */
	    void updateAccessibilityDisplay();

        /**
         * Configure the next frame to be drawn.
         */
        void render();

        /**
         * Constructor.
         *
         * Create a software representation of an LED matrix.
         * The display is initially blank.
         *
         * @param displayTimer The NRF52 timer to use to driver the LEDMatrix display
         * @param map The mapping information that relates pin inputs/outputs to physical screen coordinates.
         * @param id The id the display should use when sending events on the MessageBus. Defaults to DEVICE_ID_DISPLAY.
         * @param mode The DisplayMode to use. Default: DISPLAY_MODE_BLACK_AND_WHITE.
         */
        NRF52LEDMatrix(NRFLowLevelTimer &displayTimer, const MatrixMap &map, uint16_t id = DEVICE_ID_DISPLAY, DisplayMode mode = DisplayMode::DISPLAY_MODE_BLACK_AND_WHITE);

         /**
         * Configures the mode of the display.
         *
         * @param mode The mode to swap the display into. One of: DISPLAY_MODE_GREYSCALE,
         *             DISPLAY_MODE_BLACK_AND_WHITE, DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE
         *
         * @code
         * display.setDisplayMode(DISPLAY_MODE_GREYSCALE); //per pixel brightness
         * @endcode
         */
        void setDisplayMode(DisplayMode mode);

        /**
         * Retrieves the mode of the display.
         *
         * @return the current mode of the display
         */
        DisplayMode getDisplayMode();

        /**
         * Rotates the display to the given position.
         *
         * Axis aligned values only.
         *
         * @code
         * display.rotateTo(DISPLAY_ROTATION_180); //rotates 180 degrees from original orientation
         * @endcode
         */
        void rotateTo(DisplayRotation position);

        /**
         * Enables the display, should only be called if the display is disabled.
         *
         * @code
         * display.enable(); //Enables the display mechanics
         * @endcode
         *
         * @note Only enables the display if the display is currently disabled.
         */
        virtual void enable();

        /**
         * Disables the display, which releases control of the GPIO pins used by the display,
         * which are exposed on the edge connector.
         *
         * @code
         * display.disable(); //disables the display
         * @endcode
         *
         * @note Only disables the display if the display is currently enabled.
         */
        virtual void disable();

        /**
         * Clears the display of any remaining pixels.
         *
         * `display.image.clear()` can also be used!
         *
         * @code
         * display.clear(); //clears the display
         * @endcode
         */
        void clear();

        /**
         * Configures the brightness of the display.
         *
         * @param b The brightness to set the brightness to, in the range 0 - 255.
         *
         * @return DEVICE_OK, or DEVICE_INVALID_PARAMETER
         */
        int setBrightness(int b);

        /**
         * Determines the last ambient light level sensed.
         *
         * @return The light level sensed, as an unsigned 8-bit value in the range 0..255
         */
        int readLightLevel();

        /**
         * Puts the component in (or out of) sleep (low power) mode.
         */
        virtual int setSleep(bool doSleep) override;
        
        /**
         * Destructor for CodalDisplay, where we deregister this instance from the array of system components.
         */
        ~NRF52LEDMatrix();
    };
}

#endif
