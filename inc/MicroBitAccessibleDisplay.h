#ifndef MICROBIT_ACCESSIBLE_DISPLAY_H
#define MICROBIT_ACCESSIBLE_DISPLAY_H

#include "CodalConfig.h"
#include "NRF52Pin.h"

class MicroBitAccessibleDisplay
{

public:
    MicroBitAccessibleDisplay(NRF52Pin &pin, const uint16_t num_leds);
    ~MicroBitAccessibleDisplay();

	void set_pixel(uint16_t index, uint8_t value);
    void set_color(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);
	void set_color_base(uint8_t red, uint8_t green, uint8_t blue);
    void update();
	void clear();

private:
    NRF52Pin &pin;
	uint16_t num_leds;
    uint8_t *buffer;

	uint8_t base_red;
	uint8_t base_green;
	uint8_t base_blue;
};

#endif
