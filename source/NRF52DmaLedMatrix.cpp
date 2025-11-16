#include "NRF52DmaLedMatrix.h"
#include "NRF52Pin.h"
#include "CodalDmesg.h"
#include "ErrorNo.h"
#include "nrf.h"
#include "codal-core/inc/driver-models/Timer.h"

using namespace codal;

static NRF52DmaLEDMatrix *instance = NULL;

static void display_irq(uint16_t mask)
{
    if (instance)
        instance->render();
}

NRF52DmaLEDMatrix::NRF52DmaLEDMatrix(NRFLowLevelTimer &timer, const MatrixMap &map, uint16_t id, DisplayMode mode) : Display(map.width, map.height, id), matrixMap(map), timer(timer)
{
    this->rotation = MATRIX_DISPLAY_ROTATION_0;
    this->enabled = false;
    this->strobeRow = 0;
    this->mode = mode;

    instance = this;
}

void NRF52DmaLEDMatrix::enable()
{
    if(enabled)
        return;

    this->setDisplayMode(this->mode);

    for(int i=0; i < matrixMap.rows; i++)
    {
        matrixMap.rowPins[i]->setDigitalValue(0);
        matrixMap.rowPins[i]->setPinMode(PinMode::DigitalOut);
    }

    NRF_PWM1->PSEL.OUT[0] = matrixMap.columnPins[0]->name;
    NRF_PWM1->PSEL.OUT[1] = matrixMap.columnPins[1]->name;
    NRF_PWM1->PSEL.OUT[2] = matrixMap.columnPins[2]->name;
    NRF_PWM1->PSEL.OUT[3] = matrixMap.columnPins[3]->name;

    NRF_PWM1->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
    NRF_PWM1->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
    NRF_PWM1->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);
    NRF_PWM1->COUNTERTOP = 255;
    NRF_PWM1->LOOP = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);
    NRF_PWM1->DECODER = (PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos) | (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
    NRF_PWM1->SEQ[0].PTR = (uint32_t)pwm_buffer;
    NRF_PWM1->SEQ[0].CNT = 4;
    NRF_PWM1->SEQ[0].REFRESH = 0;
    NRF_PWM1->SEQ[0].ENDDELAY = 0;

    timer.setMode(TimerMode::TimerModeTimer);
    timer.setClockSpeed(1000);
    timer.timer_pointer = display_irq;
    timer.setCompare(0, 4);
    timer.enable();
    timer.enableIRQ();

    enabled = true;
}

void NRF52DmaLEDMatrix::disable()
{
    if(!enabled)
        return;

    timer.disable();
    timer.disableIRQ();
    NRF_PWM1->ENABLE = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);

    enabled = false;
}

void NRF52DmaLEDMatrix::render()
{
    uint8_t *buffer = image.getBitmap();

    matrixMap.rowPins[strobeRow]->setDigitalValue(0);

    strobeRow = (strobeRow + 1) % matrixMap.rows;

    MatrixPoint *p = (MatrixPoint *)matrixMap.map + strobeRow;

    for(int i = 0; i < matrixMap.columns; i++)
    {
        uint8_t value = buffer[p->y * width + p->x];
        pwm_buffer[i] = value;
        p+=matrixMap.rows;
    }

    NRF_PWM1->TASKS_SEQSTART[0] = 1;

    matrixMap.rowPins[strobeRow]->setDigitalValue(1);
}

void NRF52DmaLEDMatrix::setDisplayMode(DisplayMode m)
{
    this->mode = m;
}

DisplayMode NRF52DmaLEDMatrix::getDisplayMode()
{
    return this->mode;
}

void NRF52DmaLEDMatrix::rotateTo(DisplayRotation r)
{
    this->rotation = r;
}

void NRF52DmaLEDMatrix::clear()
{
    image.clear();
}

int NRF52DmaLEDMatrix::setBrightness(int b)
{
    return Display::setBrightness(b);
}

int NRF52DmaLEDMatrix::readLightLevel()
{
    return 0;
}

int NRF52DmaLEDMatrix::setSleep(bool doSleep)
{
    return DEVICE_OK;
}

NRF52DmaLEDMatrix::~NRF52DmaLEDMatrix()
{
}