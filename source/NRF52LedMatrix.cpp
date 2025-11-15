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
    rotation = MATRIX_DISPLAY_ROTATION_0;
    enabled = false;
    strobeRow = 0;
    instance = this;
    lightLevel = 0;
    is_dirty_ = true;
    image_checksum_ = 0xFFFFFFFF;
    display_active_ = true;

    for (int i = 0; i < 5; i++)
        for (int j = 0; j < NRF52_LED_MATRIX_MAXIMUM_COLUMNS; j++)
            rendered_image_[i][j] = 0;
    this->mode = mode;

    // Validate that we can deliver the requested display.
    if (matrixMap.columns <= NRF52_LED_MATRIX_MAXIMUM_COLUMNS)
    {
        // Configure as a fixed period timer
        timer.setMode(TimerMode::TimerModeTimer);
        timer.setClockSpeed(NRF52_LED_MATRIX_CLOCK_FREQUENCY/1000);
        timer.timer_pointer = display_irq;

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
    if (!enabled || (status & NRF52_LEDMATRIX_STATUS_RESET))
    {
        for (int channel = 0; channel < matrixMap.columns; channel++)
        {
            gpiote[channel] = NRF52_LEDMATRIX_GPIOTE_CHANNEL_BASE + channel;
            ppi[channel] = NRF52_LEDMATRIX_PPI_CHANNEL_BASE + channel;

            NRF_GPIOTE->CONFIG[gpiote[channel]] = 0x00010003 | (matrixMap.columnPins[channel]->name << 8);
            NRF_PPI->CH[ppi[channel]].EEP = (uint32_t) &timer.timer->EVENTS_COMPARE[channel+1];
            NRF_PPI->CH[ppi[channel]].TEP = (uint32_t) &NRF_GPIOTE->TASKS_SET[gpiote[channel]];
            NRF_PPI->CHENSET = 1 << ppi[channel];
        }
        status &= ~NRF52_LEDMATRIX_STATUS_RESET;
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
  * Rotates the display to the given position.
  *
  * Axis aligned values only.
  *
  * @code
  * display.rotateTo(MATRIX_DISPLAY_ROTATION_180); //rotates 180 degrees from original orientation
  * @endcode
  */
void NRF52LEDMatrix::rotateTo(DisplayRotation rotation)
{
    this->rotation = rotation;
    is_dirty_ = true;
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

    // Configure for the requested mode.
    this->setDisplayMode(mode);

    // Ensure row drive pins are disabled, and have no pull resistors configured
    for (int row = 0; row < matrixMap.rows; row++)
        matrixMap.rowPins[row]->getDigitalValue(PullMode::None);

    // Ensure column drive pins are disabled, and have no pull resistors configured
    for (int col = 0; col < matrixMap.columns; col++)
        matrixMap.columnPins[col]->getDigitalValue(PullMode::None);

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

    // Disable the timer that drivers the display
    timer.disable();
    timer.disableIRQ();

    // Disable GPIOTE control of the display pins
    // FIXME: When GPIOTE is disabled here "system off" consumes almost 1mA @ 3V
    // It's unclear how changing this peripheral causes the additional power consumption
    // https://github.com/lancaster-university/codal-microbit-v2/issues/30
    for (int column = 0; column < matrixMap.columns; column++)
        NRF_GPIOTE->CONFIG[gpiote[column]] = 0;


    // Put all pins into high impedance mode.
    for (int column = 0; column < matrixMap.columns; column++)
         matrixMap.columnPins[column]->getDigitalValue();

    for (int row = 0; row < matrixMap.rows; row++)
         matrixMap.rowPins[row]->getDigitalValue();

    status &= ~NRF52_LEDMATRIX_STATUS_LIGHTREADY;

    enabled = false;
}

/**
 * Configure the next frame to be drawn.
 */
void NRF52LEDMatrix::render()
{
    if (strobeRow == 0)
    {
        uint32_t checksum = calculate_checksum();

        if (checksum == 0)
        {
            if (display_active_)
            {
                timer.disable();
                timer.disableIRQ();
                display_active_ = false;

                // also turn off the last row, as we may be mid-frame.
                matrixMap.rowPins[matrixMap.rows-1]->setDigitalValue(0);
            }
        }
        else
        {
            if (!display_active_)
            {
                timer.enable();
                timer.enableIRQ();
                display_active_ = true;
            }
        }

        if (checksum != image_checksum_)
        {
            is_dirty_ = true;
            image_checksum_ = checksum;
        }
    }

    if (!display_active_)
        return;

    if (is_dirty_)
    {
        uint8_t *screenBuffer = image.getBitmap();
        uint32_t value;

        for (int row = 0; row < matrixMap.rows; row++)
        {
            MatrixPoint *p = (MatrixPoint *)matrixMap.map + row;

            for (int column = 0; column < matrixMap.columns; column++)
            {
                switch (this->rotation)
                {
                    case MATRIX_DISPLAY_ROTATION_0:
                        value = screenBuffer[p->y * width + p->x];
                        break;
                    case MATRIX_DISPLAY_ROTATION_90:
                        value = screenBuffer[p->x * width + width - 1 - p->y];
                        break;
                    case MATRIX_DISPLAY_ROTATION_180:
                        value = screenBuffer[(height - 1 - p->y) * width + width - 1 - p->x];
                        break;
                    case MATRIX_DISPLAY_ROTATION_270:
                        value = screenBuffer[(height - 1 - p->x) * width + p->y];
                        break;
                    default:
                        value = screenBuffer[p->y * width + p->x];
                        break;
                }

                if (mode == DISPLAY_MODE_BLACK_AND_WHITE || mode == DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE)
                    value = value ? 255 : 0;

                rendered_image_[row][column] = value * quantum;
                p += matrixMap.rows;
            }
        }
        is_dirty_ = false;
    }

    if (strobeRow < matrixMap.rows)
    {
        matrixMap.rowPins[strobeRow]->setDigitalValue(0);
    }
    else
    {
        lightLevel = 255 - ((255 * timer.timer->CC[1]) / (timerPeriod * NRF52_LED_MATRIX_LIGHTSENSE_STROBES));
        status |= NRF52_LEDMATRIX_STATUS_LIGHTREADY;
        status |= NRF52_LEDMATRIX_STATUS_RESET;
        setDisplayMode(mode);
    }
    
    timer.timer->TASKS_STOP = 1;
    strobeRow = (strobeRow + 1) % timeslots;

    if(strobeRow < matrixMap.rows)
    {
        for (int column = 0; column < matrixMap.columns; column++)
        {
            uint32_t value = rendered_image_[strobeRow][column];
            timer.timer->CC[column+1] = value;

            if (value)
                NRF_GPIOTE->CONFIG[gpiote[column]] &= ~0x00100000;
            else
                NRF_GPIOTE->CONFIG[gpiote[column]] |= 0x00100000;
        }
        matrixMap.rowPins[strobeRow]->setDigitalValue(1);
    }
    else
    {
        timer.setCompare(0, timerPeriod * NRF52_LED_MATRIX_LIGHTSENSE_STROBES);
       
        for (int column = 0; column < matrixMap.columns; column++)
        {
            NRF_GPIOTE->CONFIG[gpiote[column]] = 0;
            timer.timer->CC[column+1] = timerPeriod * NRF52_LED_MATRIX_LIGHTSENSE_STROBES;
            matrixMap.columnPins[column]->setDigitalValue(1);
        }

        matrixMap.rowPins[0]->setDigitalValue(0);
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
    is_dirty_ = true;
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

    is_dirty_ = true;

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
    {
        setDisplayMode(DisplayMode::DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE);
        status &= ~NRF52_LEDMATRIX_STATUS_LIGHTREADY;
    }

    if (mode == DisplayMode::DISPLAY_MODE_GREYSCALE)
    {
        setDisplayMode(DisplayMode::DISPLAY_MODE_GREYSCALE_LIGHT_SENSE);
        status &= ~NRF52_LEDMATRIX_STATUS_LIGHTREADY;
    }

    // if we've just enabled light sensing, ensure we have a valid reading before returning.
    if ( ( status & NRF52_LEDMATRIX_STATUS_LIGHTREADY) == 0)
        fiber_sleep(1500.0f/((float)NRF52_LED_MATRIX_FREQUENCY));

    return lightLevel;
}

/**
 * Puts the component in (or out of) sleep (low power) mode.
 */
int NRF52LEDMatrix::setSleep(bool doSleep)
{
    static bool wasEnabled;

    if (doSleep)
    {
        wasEnabled = enabled;
        disable();
    }

    if (!doSleep && wasEnabled)
    {
        enable();
    }
   
    return DEVICE_OK;
}

/**
 * Calculates a checksum of the image buffer.
 */
uint32_t NRF52LEDMatrix::calculate_checksum()
{
    uint32_t checksum = 0;
    uint8_t *buffer = image.getBitmap();

    for (int i = 0; i < width * height; i++)
    {
        checksum = (checksum >> 1) | (checksum << 31);
        checksum += buffer[i];
    }

    return checksum;
}

/**
  * Destructor for LEDMatrix, where we deregister this instance from the array of system components.
  */
NRF52LEDMatrix::~NRF52LEDMatrix()
{
    this->status &= ~DEVICE_COMPONENT_STATUS_SYSTEM_TICK;
}