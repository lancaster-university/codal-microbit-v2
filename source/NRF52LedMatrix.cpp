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

/**
  * Class definition for LEDMatrix.
  *
  * A LEDMatrix represents the LED matrix array on the micro:bit.
  */
#include "NRF52LedMatrix.h"
#include "NRF52Pin.h"
#include "CodalDmesg.h"
#include "ErrorNo.h"

using namespace codal;

static NRF52LEDMatrix *instance = NULL;

static void display_irq(uint16_t mask)
{
    if (instance)
        instance->render();
}

/**
 * Constructor.
 *
 * Create a software representation an LED matrix.
 * The display is initially blank.
 *
 * @param displayTimer The NRF52 timer to use to driver the LEDMatrix display.
 * @param map The mapping information that relates pin inputs/outputs to physical screen coordinates.
 * @param id The id the display should use when sending events on the MessageBus. Defaults to DEVICE_ID_DISPLAY.
 * @param mode The DisplayMode to use. Default: DISPLAY_MODE_BLACK_AND_WHITE.
 */
NRF52LEDMatrix::NRF52LEDMatrix(NRFLowLevelTimer &displayTimer, const MatrixMap &map, uint16_t id, DisplayMode mode) : Display(map.width, map.height, id), matrixMap(map), timer(displayTimer)
{
    enabled = false;
    strobeRow = 0;
    instance = this;
    lightLevel = 0;

    // Validate that we can deliver the requested display.
    if (matrixMap.columns <= NRF52_LED_MATRIX_MAXIMUM_COLUMNS)
    {
        // Configure as a fixed period timer
        timer.setMode(TimerMode::TimerModeTimer);
        timer.setClockSpeed(NRF52_LED_MATRIX_CLOCK_FREQUENCY/1000);
        timer.timer_pointer = display_irq;

        // Configure for the requested mode.
        this->setDisplayMode(mode);

        // Ensure row drive pins are disabled, and have no pull resistors configured
        for (int row = 0; row < matrixMap.rows; row++)
            matrixMap.rowPins[row]->getDigitalValue(PullMode::None);

        this->enable();
    }
}

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
void NRF52LEDMatrix::setDisplayMode(DisplayMode mode)
{
    // Allocate GPIOTE and PPI channels
    // TODO: use a global allocator here, rather than static allocation.
    for (int channel = 0; channel < matrixMap.columns; channel++)
    {
        gpiote[channel] = NRF52_LEDMATRIX_GPIOTE_CHANNEL_BASE + channel;
        ppi[channel] = NRF52_LEDMATRIX_PPI_CHANNEL_BASE + channel;

        NRF_GPIOTE->CONFIG[gpiote[channel]] = 0x00010003 | (matrixMap.columnPins[channel]->name << 8);
        NRF_PPI->CH[ppi[channel]].EEP = (uint32_t) &timer.timer->EVENTS_COMPARE[channel+1];
        NRF_PPI->CH[ppi[channel]].TEP = (uint32_t) &NRF_GPIOTE->TASKS_SET[gpiote[channel]];
        NRF_PPI->CHENSET = 1 << ppi[channel]; 
    }
            
    // Determine the number of timeslots we'll need.
    timeslots = matrixMap.rows;

    if (mode == DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE || mode == DISPLAY_MODE_GREYSCALE_LIGHT_SENSE)
        timeslots++;

    timerPeriod = NRF52_LED_MATRIX_CLOCK_FREQUENCY / (NRF52_LED_MATRIX_FREQUENCY * timeslots);
    quantum = (timerPeriod * brightness) / (256 * 255);
    
    timer.setCompare(0, timerPeriod);
    timer.timer->TASKS_CLEAR = 1;

    this->mode = mode;
}

/**
 * Retrieves the mode of the display.
 *
 * @return the current mode of the display
 */
DisplayMode NRF52LEDMatrix::getDisplayMode()
{
    return this->mode;
}

/**
 * Enables the display, should only be called if the display is disabled.
 *
 * @code
 * display.enable(); //Enables the display 
 * @endcode
 *
 * @note Only enables the display if the display is currently disabled.
 */
void NRF52LEDMatrix::enable()
{
    if (enabled)
        return;

    timer.enable();
    timer.enableIRQ();

    enabled = true;
}

/**
 * Disables the display, which releases control of the GPIO pins used by the display,
 * which are exposed on the edge connector.
 *
 * @code
 * display.disable(); //disables the display
 * @endcode
 *
 */
void NRF52LEDMatrix::disable()
{
    if (!enabled)
        return;

    enabled = false;
}

/**
 * Configure the next frame to be drawn.
 */
void NRF52LEDMatrix::render()
{
    uint8_t *screenBuffer = image.getBitmap();
    uint32_t value;

    if (strobeRow < matrixMap.rows)
    {
        // We just completed a normal diplay strobe. 
        // Turn off the LED drive to the row that was completed.
        matrixMap.rowPins[strobeRow]->setDigitalValue(0);
    }
    else
    {
        // We just completed a light sense strobe.
        // Record the light level sensed, and restore the hardware configuration into LED drive mode.
        lightLevel = 255 - ((255 * timer.timer->CC[1]) / (timerPeriod * NRF52_LED_MATRIX_LIGHTSENSE_STROBES));
        setDisplayMode(mode);
    }
    
    // Stop the timer temporarily, to avoid possible race conditions.
    timer.timer->TASKS_STOP = 1;

    // Move on to the next row.
    strobeRow = (strobeRow + 1) % timeslots;

    if(strobeRow < matrixMap.rows)
    {
        // Common case - configure timer values.
        MatrixPoint *p = (MatrixPoint *)matrixMap.map + strobeRow;

        for (int column = 0; column < matrixMap.columns; column++)
        {
            value = screenBuffer[p->y * width + p->x];

            // Clip pixels to full or zero brightness if in black and white mode.
            if (mode == DISPLAY_MODE_BLACK_AND_WHITE || mode == DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE)
                value = value ? 255 : 0;

            value = value * quantum;
            timer.timer->CC[column+1] = value;

            // Set the initial polarity of the column output to HIGH if the pixel brightness is >0. LOW otherwise.
            if (value)
                NRF_GPIOTE->CONFIG[gpiote[column]] &= ~0x00100000;
            else
                NRF_GPIOTE->CONFIG[gpiote[column]] |= 0x00100000;
            
            p += matrixMap.rows;
        }

        // Enable the drive pin, and start the timer.
        matrixMap.rowPins[strobeRow]->setDigitalValue(1);
    }
    else
    {
        // Perform Light sensing. This is tricky, as we need to reconfigure the timer, PPI and GPIOTE channels to
        // sense and capture the voltage on the LED rows, rather than drive them.
        
        // Extend the refresh period to allow for reasonable accuracy.
        timer.setCompare(0, timerPeriod * NRF52_LED_MATRIX_LIGHTSENSE_STROBES);
       
        // Disable GPIOTE control on the columns pins, and set all column pins to HIGH.
        // n.b. we don't use GPIOTE to do this drive as we need to reuse the channels anyway...
        for (int column = 0; column < matrixMap.columns; column++)
        {
            NRF_GPIOTE->CONFIG[gpiote[column]] = 0;
            timer.timer->CC[column+1] = timerPeriod * NRF52_LED_MATRIX_LIGHTSENSE_STROBES;
            matrixMap.columnPins[column]->setDigitalValue(1);
        }

        // Pull the sense pin low 
        matrixMap.rowPins[0]->setDigitalValue(0);

        // Configure GPIOTE and PPI to measure the sense pin rise time.
        NRF_GPIOTE->CONFIG[gpiote[0]] = 0x00010001 | (matrixMap.rowPins[0]->name << 8);
        NRF_PPI->CH[ppi[0]].EEP = (uint32_t) &NRF_GPIOTE->EVENTS_IN[gpiote[0]];
        NRF_PPI->CH[ppi[0]].TEP = (uint32_t) &timer.timer->TASKS_CAPTURE[1];
    }
    
    timer.timer->TASKS_CLEAR = 1;
    timer.timer->TASKS_START = 1;
}

/**
  * Clears the display of any remaining pixels.
  *
  * `display.image.clear()` can also be used!
  *
  * @code
  * display.clear(); //clears the display
  * @endcode
  */
void NRF52LEDMatrix::clear()
{
    image.clear();
}

/**
 * Configures the brightness of the display.
 *
 * @param b The brightness to set the brightness to, in the range 0 - 255.
 *
 * @return DEVICE_OK, or DEVICE_INVALID_PARAMETER
 */
int NRF52LEDMatrix::setBrightness(int b)
{
    int result = Display::setBrightness(b);

    if (result != DEVICE_OK)
        return result;

    // Recalculate our quantum based on the new brightness setting.
    quantum = (timerPeriod * brightness) / (256 * 255);

    return DEVICE_OK;
}

/**
 * Determines the last ambient light level sensed.
 *
 * @return The light level sensed, as an unsigned 8-bit value in the range 0..255
 */
int 
NRF52LEDMatrix::readLightLevel()
{
    // Auto-enable light sensing if it is currently disabled
    if (mode == DisplayMode::DISPLAY_MODE_BLACK_AND_WHITE)
        setDisplayMode(DisplayMode::DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE);

    if (mode == DisplayMode::DISPLAY_MODE_GREYSCALE)
        setDisplayMode(DisplayMode::DISPLAY_MODE_GREYSCALE_LIGHT_SENSE);

    return lightLevel;
}

/**
  * Destructor for LEDMatrix, where we deregister this instance from the array of system components.
  */
NRF52LEDMatrix::~NRF52LEDMatrix()
{
    this->status &= ~DEVICE_COMPONENT_STATUS_SYSTEM_TICK;
}