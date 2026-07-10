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
    timeslots = matrixMap.rows + 1;

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
    uint8_t *screenBuffer = image.getBitmap();
    uint32_t value;
    uint16_t index;

    if (strobeRow < matrixMap.rows)
    {
        // We just completed a normal diplay strobe. 
        // Turn off the LED drive to the row that was completed.
        matrixMap.rowPins[strobeRow]->setDigitalValue(0);
    }
    else if (strobeRow == NRF52_LED_MATRIX_NEOPIXEL_TIMESLOT)
    {
        timer.setCompare(0, timerPeriod);

        // Restore the hardware configuration into LED drive mode.
        status |= NRF52_LEDMATRIX_STATUS_RESET;
        setDisplayMode(mode);
    }
    else
    {
        // We just completed a light sense strobe. Record the light level sensed.
        lightLevel = 255 - ((255 * timer.timer->CC[1]) / (timerPeriod * NRF52_LED_MATRIX_LIGHTSENSE_STROBES));
        status |= NRF52_LEDMATRIX_STATUS_LIGHTREADY;

        // Restore the hardware configuration into LED drive mode.
        status |= NRF52_LEDMATRIX_STATUS_RESET;
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
            switch ( this->rotation)
            {
              case MATRIX_DISPLAY_ROTATION_0:
                index = p->y * width + p->x;
                break;
              case MATRIX_DISPLAY_ROTATION_90:
                index = p->x * width + width - 1 - p->y;
                break;
              case MATRIX_DISPLAY_ROTATION_180:
                index = (height - 1 - p->y) * width + width - 1 - p->x;
                break;
              case MATRIX_DISPLAY_ROTATION_270:
                index = ( height - 1 - p->x) * width + p->y;
                break;
              default:
                index = p->y * width + p->x;
                break;
            }

            value = screenBuffer[index];

            // Clip pixels to full or zero brightness if in black and white mode.
            if (mode == DISPLAY_MODE_BLACK_AND_WHITE || mode == DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE)
                value = value ? 255 : 0;

            value = value * quantum;
            timer.timer->CC[column+1] = value;

            // Update the accessibiity neopixel buffer for this pixel.
            // We do this here in the display strobe, as this gives us a more consistent timing.
            setAccessibilityPixel(index, value);

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
    else if(strobeRow == NRF52_LED_MATRIX_NEOPIXEL_TIMESLOT)
    {
        for (int col = 0; col < matrixMap.columns; col++)
        {
            NRF_GPIOTE->CONFIG[gpiote[col]] = 0;
            timer.timer->CC[col + 1] = timerPeriod * NRF52_LED_MATRIX_LIGHTSENSE_STROBES;
            matrixMap.rowPins[col]->setDigitalValue(0);
        }

        // Put all pins into high impedance mode.
        for (int row = 0; row < matrixMap.rows; row++)
            matrixMap.columnPins[row]->getDigitalValue();

        // Update the accessibility display
        updateAccessibilityDisplay();

        // We can calculate the period of time it will take the PWM to clock out the data in microseconds using:
        // (300 + 32 * numberOfPixels). he timer is a 16Mhz timer however so we multiple by 16 to get a tickcount.
        timer.setCompare(0, 16 * (150 + 10 * accessibilityBuf.length()));
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
 * Configures an optional WS2812B driven accesibility display for this NRF52LedMatrix.
 *
 * @param pin A reference to the pin object to drive the display from
 * @param numberOfLeds The total number of LEDs to drive. Defaults to 25
 */
void NRF52LEDMatrix::setAccessibilityDisplay(NRF52Pin &pin, const uint16_t numberOfLeds)
{
    accessibilityPin = &pin;

    if (accessibilityBuf.length() != numberOfLeds * 3)
        accessibilityBuf = ManagedBuffer(numberOfLeds * 3);
}

/**
 * Sets or clears the pixel at the given location on an attached accessibility display.
 *
 * @param index Zero based index of the pixel to change.
 * @param value 8 bit brightness data for that pixel.
 */
void NRF52LEDMatrix::setAccessibilityPixel(uint16_t index, uint8_t value)
{
    if (index >= accessibilityBuf.length())
        return;

    // WS2812B wire order is Green, Red, Blue. We store in this order to avoid having to reorder when we send.
    accessibilityBuf[index * 3 + 0] = (accessibilityColour.green / 255.0) * value;
    accessibilityBuf[index * 3 + 1] = (accessibilityColour.red / 255.0) * value;
    accessibilityBuf[index * 3 + 2] = (accessibilityColour.blue / 255.0) * value;
}

/**
 * Define the pixel colour to use for WS2812B attached accessibilty display.
 * Accessibility needs may require colours other than red for usability.
 *
 * @param red The red component of the colour to use.
 * @param green The red component of the colour to use.
 * @param blue The red component of the colour to use.
 */
void NRF52LEDMatrix::setAccessibilityBaseColour(Colour colour)
{
    accessibilityColour = colour;
}

/**
 * Enable accessibility mode
 */
void NRF52LEDMatrix::enableAccessibility(bool enable)
{
    this->accessibilityEnabled = enable;
}

/**
 * Refresh the attached WS2812B accessibility display.
 */
void NRF52LEDMatrix::updateAccessibilityDisplay()
{
    static bool wasEnabled = false;

    if (this->accessibilityEnabled)
    {
        if (pwm == NULL)
        {
            ws = new WS2812B();
            pwm = new NRF52PWM(NRF_PWM3, *ws, WS2812B_PWM_FREQ);

            pwm->setStreamingMode(true, false);
            pwm->setDecoderMode(PWM_DECODER_LOAD_Common);
            pwm->setSampleRate(WS2812B_PWM_FREQ);

            accessibilityPin = (NRF52Pin *) matrixMap.accessibility;
            accessibilityBuf = ManagedBuffer(25 * 3);
        }

        wasEnabled = true;
        pwm->connectPin(*accessibilityPin, 0);
        ws->playAsync(accessibilityBuf);
    }
    else if (wasEnabled)
    {
        accessibilityBuf.fill(0);
        pwm->connectPin(*accessibilityPin, 0);
        ws->playAsync(accessibilityBuf);
        wasEnabled = false;
    }
}

/**
  * Destructor for LEDMatrix, where we deregister this instance from the array of system components.
  */
NRF52LEDMatrix::~NRF52LEDMatrix()
{
    this->status &= ~DEVICE_COMPONENT_STATUS_SYSTEM_TICK;
}
