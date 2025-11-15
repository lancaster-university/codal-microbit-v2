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
IMPLIED, INCLUDING BUT NOTLIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "NRF52DmaLedMatrix.h"
#include "codal-core/inc/core/ErrorNo.h"

#define NRF52_LED_MATRIX_FREQUENCY          100
#define NRF52_LED_MATRIX_MAX_COLUMNS      5
#define NRF52_LED_MATRIX_MAX_ROWS         5
#define PWM_MAX_DUTY                        255

namespace codal
{

/**
 * Constructor.
 */
NRF52DmaLEDMatrix::NRF52DmaLEDMatrix(const MatrixMap &map, NRFLowLevelTimer &timer, uint16_t id) : Display(map.width, map.height, id), matrixMap(map), timer(timer)
{
    this->rotation = MATRIX_DISPLAY_ROTATION_0;
    this->enabled = false;
    memset(pwm_buffer, 0, sizeof(pwm_buffer));
}

/**
 * Updates the PWM buffer with the current image data.
 */
void NRF52DmaLEDMatrix::render()
{
    uint8_t *screenBuffer = image.getBitmap();

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t value;

            switch (this->rotation)
            {
                case MATRIX_DISPLAY_ROTATION_0:
                    value = screenBuffer[y * width + x];
                    break;
                case MATRIX_DISPLAY_ROTATION_90:
                    value = screenBuffer[x * width + (width - 1 - y)];
                    break;
                case MATRIX_DISPLAY_ROTATION_180:
                    value = screenBuffer[(height - 1 - y) * width + (width - 1 - x)];
                    break;
                case MATRIX_DISPLAY_ROTATION_270:
                    value = screenBuffer[(height - 1 - x) * width + y];
                    break;
            }

            if (x < 4)
                pwm_buffer[y][x] = (value * brightness) / 255;
            else
                pwm_buffer[y][4] = (value * brightness) / 255;
        }
    }
}

/**
 * Rotates the display to the given position.
 */
void NRF52DmaLEDMatrix::rotateTo(DisplayRotation rotation)
{
    this->rotation = rotation;
    render();
}

/**
 * Enables the display.
 */
void NRF52DmaLEDMatrix::enable()
{
    if (enabled)
        return;

    render();

    // Configure GPIO pins
    for (int i = 0; i < matrixMap.columns; i++)
        matrixMap.columnPins[i]->setDigitalValue(0);

    for (int i = 0; i < matrixMap.rows; i++)
        matrixMap.rowPins[i]->setDigitalValue(1);

    // Configure PWM0 for the first 4 columns
    NRF_PWM0->PSEL.OUT[0] = matrixMap.columnPins[0]->name;
    NRF_PWM0->PSEL.OUT[1] = matrixMap.columnPins[1]->name;
    NRF_PWM0->PSEL.OUT[2] = matrixMap.columnPins[2]->name;
    NRF_PWM0->PSEL.OUT[3] = matrixMap.columnPins[3]->name;
    NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
    NRF_PWM0->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
    NRF_PWM0->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);
    NRF_PWM0->COUNTERTOP = (PWM_MAX_DUTY << PWM_COUNTERTOP_COUNTERTOP_Pos);
    NRF_PWM0->LOOP = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);
    NRF_PWM0->DECODER = (PWM_DECODER_LOAD_Grouped << PWM_DECODER_LOAD_Pos) | (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
    NRF_PWM0->SEQ[0].PTR = (uint32_t)pwm_buffer[0];
    NRF_PWM0->SEQ[0].CNT = 4;
    NRF_PWM0->SEQ[0].REFRESH = 0;
    NRF_PWM0->SEQ[0].ENDDELAY = 0;

    // Configure PWM1 for the 5th column
    NRF_PWM1->PSEL.OUT[0] = matrixMap.columnPins[4]->name;
    NRF_PWM1->PSEL.OUT[1] = 0xFFFFFFFF;
    NRF_PWM1->PSEL.OUT[2] = 0xFFFFFFFF;
    NRF_PWM1->PSEL.OUT[3] = 0xFFFFFFFF;
    NRF_PWM1->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
    NRF_PWM1->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
    NRF_PWM1->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);
    NRF_PWM1->COUNTERTOP = (PWM_MAX_DUTY << PWM_COUNTERTOP_COUNTERTOP_Pos);
    NRF_PWM1->LOOP = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);
    NRF_PWM1->DECODER = (PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos) | (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
    NRF_PWM1->SEQ[0].PTR = (uint32_t)&pwm_buffer[0][4];
    NRF_PWM1->SEQ[0].CNT = 1;
    NRF_PWM1->SEQ[0].REFRESH = 0;
    NRF_PWM1->SEQ[0].ENDDELAY = 0;

    // Configure GPIOTE to set row pins
    for (int i = 0; i < matrixMap.rows; i++) {
        NRF_GPIOTE->CONFIG[i] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos) |
                                (matrixMap.rowPins[i]->name << GPIOTE_CONFIG_PSEL_Pos) |
                                (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos) |
                                (GPIOTE_CONFIG_OUTINIT_High << GPIOTE_CONFIG_OUTINIT_Pos);
    }
    NRF_GPIOTE->CONFIG[5] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos) | (GPIOTE_CONFIG_POLARITY_LoToHi << GPIOTE_CONFIG_POLARITY_Pos) | (GPIOTE_CONFIG_OUTINIT_High << GPIOTE_CONFIG_OUTINIT_Pos);
    for(int i = 0; i < matrixMap.rows; i++)
        NRF_GPIOTE->CONFIG[5] |= (1 << (GPIOTE_CONFIG_PSEL_Pos + i));

    // Configure Timer to trigger row changes
    timer.setMode(TimerMode::TimerModeTimer);
    timer.setClockSpeed(1000000); // 1MHz
    timer.setCompare(0, 1000000 / (NRF52_LED_MATRIX_FREQUENCY * matrixMap.rows));

    // Configure PPI
    for (int i = 0; i < matrixMap.rows; i++) {
        // On timer compare, set the current row low
        NRF_PPI->CH[i].EEP = (uint32_t)&timer.timer->EVENTS_COMPARE[0];
        NRF_PPI->CH[i].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[i];

        // On PWM sequence end, update the PWM sequence pointers and start the sequences for the next row
        NRF_PPI->CH[5 + i].EEP = (uint32_t)&NRF_PWM0->EVENTS_SEQEND[0];
        NRF_PPI->CH[5 + i].TEP = (uint32_t)&NRF_PWM0->TASKS_SEQSTART[0];
        NRF_PPI->FORK[5 + i].TEP = (uint32_t)&NRF_PWM1->TASKS_SEQSTART[0];
        NRF_PWM0->SEQ[0].PTR = (uint32_t)pwm_buffer[(i + 1) % matrixMap.rows];
        NRF_PWM1->SEQ[0].PTR = (uint32_t)&pwm_buffer[(i + 1) % matrixMap.rows][4];
    }
    // After the last row, clear all rows
    NRF_PPI->CH[10].EEP = (uint32_t)&NRF_PWM0->EVENTS_SEQEND[0];
    NRF_PPI->CH[10].TEP = (uint32_t)&NRF_GPIOTE->TASKS_CLR[5];

    NRF_TIMER4->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);

    for(int i = 0; i < 11; i++)
        NRF_PPI->CHENSET = (1 << i);

    timer.enable();

    enabled = true;
}

/**
 * Disables the display.
 */
void NRF52DmaLEDMatrix::disable()
{
    if (!enabled)
        return;

    timer.disable();

    NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);
    NRF_PWM1->ENABLE = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);

    for (int i = 0; i < 11; i++) {
        NRF_PPI->CHENCLR = (1 << i);
    }

    for (int i = 0; i < matrixMap.rows + 1; i++) {
        NRF_GPIOTE->CONFIG[i] = 0;
    }

    for (int i = 0; i < matrixMap.columns; i++)
        matrixMap.columnPins[i]->getDigitalValue();

    for (int i = 0; i < matrixMap.rows; i++)
         matrixMap.rowPins[i]->getDigitalValue();

    enabled = false;
}

/**
 * Sets the brightness of the display.
 */
int NRF52DmaLEDMatrix::setBrightness(int b)
{
    int result = Display::setBrightness(b);
    if (result == DEVICE_OK)
    {
        render();
    }
    return result;
}

}