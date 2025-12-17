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

/**
  * Class definition for LEDMatrix.
  *
  * A LEDMatrix represents the LED matrix array on the micro:bit.
  *
  * This is a DMA-based driver, which operates completely autonomously from the CPU
  * after being configured.
  */
#include "NRF52DmaLedMatrix.h"
#include "NRF52Pin.h"
#include "CodalDmesg.h"
#include "ErrorNo.h"
#include "NRF52Dma.h"
#include "nrf.h"
#include "nrf_gpio.h"

using namespace codal;

static NRF52DmaLEDMatrix *instance = NULL;


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
NRF52DmaLEDMatrix::NRF52DmaLEDMatrix(const MatrixMap &map, uint16_t id, DisplayMode mode) : Display(map.width, map.height, id), matrixMap(map)
{
    this->rotation = MATRIX_DISPLAY_ROTATION_0;
    this->enabled = false;
    this->strobeRow = 0;
    this->lightLevel = 0;
    this->mode = mode;
    instance = this;

    // We need to write a logical 1 to the GPIO OUT register to turn an LED on,
    // so we build a DMA buffer to do this for each of our 9 strobes.
    for (int i=0; i<9; i++)
        dmaBuffer[i] = 0x00010000;

    // Configure the nrf52 port as a GPIO port, with high drive stength.
    // This gives us brighter LEDs and more linear brightness curves.
    for (int i=0; i<5; i++)
        nrf_gpio_cfg(matrixMap.columnPins[i]->name, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_H0H1, NRF_GPIO_PIN_NOSENSE);

    // Initialise the display.
    this->enable();
}

/**
 * Configures the mode of the display.
 *
 * @param mode The mode to swap the display into. One of: DISPLAY_MODE_GREYSCALE,
 *             DISPLAY_MODE_BLACK_AND_WHITE, DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE
 */
void NRF52DmaLEDMatrix::setDisplayMode(DisplayMode mode)
{
    this->mode = mode;
}

/**
 * Retrieves the mode of the display.
 *
 * @return the current mode of the display
 */
DisplayMode NRF52DmaLEDMatrix::getDisplayMode()
{
    return this->mode;
}

/**
  * Rotates the display to the given position.
  *
  * Axis aligned values only.
  */
void NRF52DmaLEDMatrix::rotateTo(DisplayRotation rotation)
{
    this->rotation = rotation;
}

/**
 * Enables the display, should only be called if the display is disabled.
 *
 * @note Only enables the display if the display is currently disabled.
 */
void NRF52DmaLEDMatrix::enable()
{
    if (enabled)
        return;

    // Timer 2 is used to control the PWM period.
    NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit;
    NRF_TIMER2->PRESCALER = 2; // 4MHz timer
    NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;

    // Timer 3 is used to control the overall display period.
    NRF_TIMER3->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER3->BITMODE = TIMER_BITMODE_BITMODE_16Bit;
    NRF_TIMER3->PRESCALER = 4; // 1MHz timer.
    NRF_TIMER3->CC[0] = 20000; // 50 Hz frame rate.
    NRF_TIMER3->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk | TIMER_SHORTS_COMPARE0_STOP_Msk;

    // Timer 4 is used purely as a source of events.
    NRF_TIMER4->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER4->BITMODE = TIMER_BITMODE_BITMODE_16Bit;
    NRF_TIMER4->PRESCALER = 4; // 1MHz timer.
    NRF_TIMER4->CC[0] = 4;     // ~250KHz
    NRF_TIMER4->CC[1] = 8;
    NRF_TIMER4->CC[2] = 12;
    NRF_TIMER4->CC[3] = 16;
    NRF_TIMER4->CC[4] = 20;
    NRF_TIMER4->CC[5] = 24;
    NRF_TIMER4->SHORTS = TIMER_SHORTS_COMPARE5_CLEAR_Msk;

    // The GPIOTE peripheral is used to control the display rows.
    // We configure a distinct channel for each row, and use these to drive the display.
    NRF_GPIOTE->CONFIG[0] = (GPIOTE_CONFIG_MODE_Task       << GPIOTE_CONFIG_MODE_Pos)     |
                            (matrixMap.rowPins[0]->name << GPIOTE_CONFIG_PSEL_Pos)     |
                            (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) |
                            (GPIOTE_CONFIG_OUTINIT_Low     << GPIOTE_CONFIG_OUTINIT_Pos);

    NRF_GPIOTE->CONFIG[1] = (GPIOTE_CONFIG_MODE_Task       << GPIOTE_CONFIG_MODE_Pos)     |
                            (matrixMap.rowPins[1]->name << GPIOTE_CONFIG_PSEL_Pos)     |
                            (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) |
                            (GPIOTE_CONFIG_OUTINIT_Low     << GPIOTE_CONFIG_OUTINIT_Pos);

    NRF_GPIOTE->CONFIG[2] = (GPIOTE_CONFIG_MODE_Task       << GPIOTE_CONFIG_MODE_Pos)     |
                            (matrixMap.rowPins[2]->name << GPIOTE_CONFIG_PSEL_Pos)     |
                            (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) |
                            (GPIOTE_CONFIG_OUTINIT_Low     << GPIOTE_CONFIG_OUTINIT_Pos);

    NRF_GPIOTE->CONFIG[3] = (GPIOTE_CONFIG_MODE_Task       << GPIOTE_CONFIG_MODE_Pos)     |
                            (matrixMap.rowPins[3]->name << GPIOTE_CONFIG_PSEL_Pos)     |
                            (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) |
                            (GPIOTE_CONFIG_OUTINIT_Low     << GPIOTE_CONFIG_OUTINIT_Pos);

    NRF_GPIOTE->CONFIG[4] = (GPIOTE_CONFIG_MODE_Task       << GPIOTE_CONFIG_MODE_Pos)     |
                            (matrixMap.rowPins[4]->name << GPIOTE_CONFIG_PSEL_Pos)     |
                            (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) |
                            (GPIOTE_CONFIG_OUTINIT_Low     << GPIOTE_CONFIG_OUTINIT_Pos);


    // The PPI peripheral is used to link all the timers and GPIO tasks together.
    // PPI channel 0 links the start of the frame (TIMER3) to the first row strobe (TIMER4)
    NRF_PPI->CH[0].EEP = (uint32_t) &NRF_TIMER3->EVENTS_COMPARE[0];
    NRF_PPI->CH[0].TEP = (uint32_t) &NRF_TIMER4->TASKS_START;
    NRF_PPI->FORK[0].TEP = (uint32_t) &NRF_GPIOTE->TASKS_CLR[0];

    // PPI channel 1 links the first row strobe to the second...
    NRF_PPI->CH[1].EEP = (uint32_t) &NRF_TIMER4->EVENTS_COMPARE[0];
    NRF_PPI->CH[1].TEP = (uint32_t) &NRF_GPIOTE->TASKS_SET[0];
    NRF_PPI->FORK[1].TEP = (uint32_t) &NRF_GPIOTE->TASKS_CLR[1];

    // PPI channel 2 links the second row strobe to the third...
    NRF_PPI->CH[2].EEP = (uint32_t) &NRF_TIMER4->EVENTS_COMPARE[1];
    NRF_PPI->CH[2].TEP = (uint32_t) &NRF_GPIOTE->TASKS_SET[1];
    NRF_PPI->FORK[2].TEP = (uint32_t) &NRF_GPIOTE->TASKS_CLR[2];

    // PPI channel 3 links the third row strobe to the fourth...
    NRF_PPI->CH[3].EEP = (uint32_t) &NRF_TIMER4->EVENTS_COMPARE[2];
    NRF_PPI->CH[3].TEP = (uint32_t) &NRF_GPIOTE->TASKS_SET[2];
    NRF_PPI->FORK[3].TEP = (uint32_t) &NRF_GPIOTE->TASKS_CLR[3];

    // PPI channel 4 links the fourth row strobe to the fifth...
    NRF_PPI->CH[4].EEP = (uint32_t) &NRF_TIMER4->EVENTS_COMPARE[3];
    NRF_PPI->CH[4].TEP = (uint32_t) &NRF_GPIOTE->TASKS_SET[3];
    NRF_PPI->FORK[4].TEP = (uint32_t) &NRF_GPIOTE->TASKS_CLR[4];

    // PPI channel 5 links the fifth row strobe to the start of the next frame.
    NRF_PPI->CH[5].EEP = (uint32_t) &NRF_TIMER4->EVENTS_COMPARE[4];
    NRF_PPI->CH[5].TEP = (uint32_t) &NRF_GPIOTE->TASKS_SET[4];
    NRF_PPI->FORK[5].TEP = (uint32_t) &NRF_TIMER4->TASKS_STOP;

    // PPI channel 6 links the PWM timer to the GPIO port task, to enable PWM of the display columns.
    NRF_PPI->CH[6].EEP = (uint32_t) &NRF_TIMER2->EVENTS_COMPARE[0];
    NRF_PPI->CH[6].TEP = (uint32_t) &NRF_GPIOTE->TASKS_OUT[5];

    // PPI channel 7 links the start of each row strobe to start the PWM timer.
    NRF_PPI->CH[7].EEP = (uint32_t) &NRF_TIMER4->EVENTS_COMPARE[5];
    NRF_PPI->CH[7].TEP = (uint32_t) &NRF_TIMER2->TASKS_START;

    // PPI channel 8-12 link each row strobe to a DMA transfer, that sets the state of the GPIO pins.
    NRF_PPI->CH[8].EEP = (uint32_t) &NRF_TIMER4->EVENTS_COMPARE[0];
    NRF_PPI->CH[8].TEP = (uint32_t) &NRF_DMA->TASKS_START[0];

    NRF_PPI->CH[9].EEP = (uint32_t) &NRF_TIMER4->EVENTS_COMPARE[1];
    NRF_PPI->CH[9].TEP = (uint32_t) &NRF_DMA->TASKS_START[1];

    NRF_PPI->CH[10].EEP = (uint32_t) &NRF_TIMER4->EVENTS_COMPARE[2];
    NRF_PPI->CH[10].TEP = (uint32_t) &NRF_DMA->TASKS_START[2];

    NRF_PPI->CH[11].EEP = (uint32_t) &NRF_TIMER4->EVENTS_COMPARE[3];
    NRF_PPI->CH[11].TEP = (uint32_t) &NRF_DMA->TASKS_START[3];

    NRF_PPI->CH[12].EEP = (uint32_t) &NRF_TIMER4->EVENTS_COMPARE[4];
    NRF_PPI->CH[12].TEP = (uint32_t) &NRF_DMA->TASKS_START[4];

    NRF_PPI->CHENSET = 0x00001FFF;

    // The DMA controller is used to write data to the GPIO pins.
    // We configure a distinct channel for each row of the display.
    // Each DMA channel is configured to write a single 32-bit value to the GPIO OUTSET register.
    NRF_DMA->ENABLE = DMA_ENABLE_ENABLE_Enabled;
    NRF_DMA->CH[0].SRC = (uint32_t) &dmaBuffer[0];
    NRF_DMA->CH[0].DST = (uint32_t) &NRF_P0->OUTCLR;
    NRF_DMA->CH[0].CNT = 1;

    NRF_DMA->CH[1].SRC = (uint32_t) &dmaBuffer[1];
    NRF_DMA->CH[1].DST = (uint32_t) &NRF_P0->OUTCLR;
    NRF_DMA->CH[1].CNT = 1;

    NRF_DMA->CH[2].SRC = (uint32_t) &dmaBuffer[2];
    NRF_DMA->CH[2].DST = (uint32_t) &NRF_P0->OUTCLR;
    NRF_DMA->CH[2].CNT = 1;

    NRF_DMA->CH[3].SRC = (uint32_t) &dmaBuffer[3];
    NRF_DMA->CH[3].DST = (uint32_t) &NRF_P0->OUTCLR;
    NRF_DMA->CH[3].CNT = 1;

    NRF_DMA->CH[4].SRC = (uint32_t) &dmaBuffer[4];
    NRF_DMA->CH[4].DST = (uint32_t) &NRF_P0->OUTCLR;
    NRF_DMA->CH[4].CNT = 1;


    // Register this component for status change events
    this->status |= DEVICE_COMPONENT_STATUS_SYSTEM_TICK;
    this->enabled = true;
}

/**
 * Disables the display, which releases control of the GPIO pins used by the display,
 * which are exposed on the edge connector.
 */
void NRF52DmaLEDMatrix::disable()
{
    if (!enabled)
        return;

    NRF_TIMER2->TASKS_STOP = 1;
    NRF_TIMER3->TASKS_STOP = 1;
    NRF_TIMER4->TASKS_STOP = 1;

    NRF_PPI->CHENCLR = 0x00001FFF;

    this->status &= ~DEVICE_COMPONENT_STATUS_SYSTEM_TICK;
    this->enabled = false;
}

/**
 * Configure the next frame to be drawn.
 */
void NRF52DmaLEDMatrix::systemTick()
{
    uint8_t *screenBuffer = image.getBitmap();
    uint32_t value;

    // The first five words of our DMA buffer are the GPIO port masks for each row.
    // We update these here, based on the current display buffer.
    for (int i=0; i<5; i++)
    {
        dmaBuffer[i] = 0;
        MatrixPoint *p = (MatrixPoint *)matrixMap.map + i;

        for (int j = 0; j < matrixMap.columns; j++)
        {
            switch ( this->rotation)
            {
                case MATRIX_DISPLAY_ROTATION_0:
                value = screenBuffer[ p->y * width + p->x];
                break;
                case MATRIX_DISPLAY_ROTATION_90:
                value = screenBuffer[ p->x * width + width - 1 - p->y];
                break;
                case MATRIX_DISPLAY_ROTATION_180:
                value = screenBuffer[ (height - 1 - p->y) * width + width - 1 - p->x];
                break;
                case MATRIX_DISPLAY_ROTATION_270:
                value = screenBuffer[ ( height - 1 - p->x) * width + p->y];
                break;
                default:
                value = screenBuffer[ p->y * width + p->x];
                break;
            }

            if(value)
                dmaBuffer[i] |= (1 << matrixMap.columnPins[j]->name);

            p += matrixMap.rows;
        }
    }

    // A little state machine to periodically start the display refresh sequence.
    if (!NRF_TIMER3->TASKS_START)
        NRF_TIMER3->TASKS_START = 1;
}

/**
  * Clears the display of any remaining pixels.
  *
  * `display.image.clear()` can also be used!
  */
void NRF52DmaLEDMatrix::clear()
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
int NRF52DmaLEDMatrix::setBrightness(int b)
{
    int result = Display::setBrightness(b);

    if (result != DEVICE_OK)
        return result;

    // The PWM period is controlled by TIMER2.
    // We scale the brightness value to the range of the timer.
    NRF_TIMER2->CC[0] = (brightness * 4);

    return DEVICE_OK;
}

/**
 * Determines the last ambient light level sensed.
 *
 * @return The light level sensed, as an unsigned 8-bit value in the range 0..255
 */
int NRF52DmaLEDMatrix::readLightLevel()
{
    // Reading the light level is not supported in DMA mode.
    return DEVICE_NOT_SUPPORTED;
}

/**
 * Puts the component in (or out of) sleep (low power) mode.
 */
int NRF52DmaLEDMatrix::setSleep(bool doSleep)
{
    if (doSleep)
        disable();
    else
        enable();

    return DEVICE_OK;
}

/**
  * Destructor for LEDMatrix, where we deregister this instance from the array of system components.
  */
NRF52DmaLEDMatrix::~NRF52DmaLEDMatrix()
{
    this->status &= ~DEVICE_COMPONENT_STATUS_SYSTEM_TICK;
}
