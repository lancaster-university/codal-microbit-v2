#include "MicroBitAccessibleDisplay.h"
#include "codal_target_hal.h"
#include "nrf_gpio.h"

MicroBitAccessibleDisplay::MicroBitAccessibleDisplay(NRF52Pin &pin, const uint16_t num_leds): pin(pin), num_leds(num_leds), buffer(NULL), base_red(40), base_green(0), base_blue(0){
    // Allocate our pixel buffer, and clear it.
    buffer = (uint8_t *)malloc(num_leds * 3);
    memset(buffer, 0, num_leds * 3);
}

MicroBitAccessibleDisplay::~MicroBitAccessibleDisplay() {
    if (buffer)
        free(buffer);
}

/* -------------------------------------------------------------------------
 * Platform helpers
 * ---------------------------------------------------------------------- */
 
/* Pins 0-31 live on P0, pins 32-47 on P1 */
static inline NRF_GPIO_Type *port_from_pin(uint8_t pin)
{
    return (pin < 32) ? NRF_P0 : NRF_P1;
}
 
static inline uint32_t bit_from_pin(uint8_t pin)
{
    return 1UL << (pin & 0x1F);
}
 
/* -------------------------------------------------------------------------
 * NOP helpers — each macro inserts exactly N pipeline stalls.
 * __NOP() is a single-cycle no-op on Cortex-M4.
 * Adjust counts to match your clock; see comments per macro.
 * ---------------------------------------------------------------------- */
 
#define NOP1()  __NOP()
#define NOP2()  NOP1();  NOP1()
#define NOP4()  NOP2();  NOP2()
#define NOP8()  NOP4();  NOP4()
#define NOP16() NOP8();  NOP8()
#define NOP32() NOP16(); NOP16()
 
/* T0H: ~300ns high */
#define DELAY_T0H()  NOP16(); NOP2();
 
/* T0L: ~900ns low */
#define DELAY_T0L()  NOP32(); NOP16();
 
/* T1H: ~900ns high */
#define DELAY_T1H()  NOP32(); NOP16();
 
/* T1L: ~300ns low */
#define DELAY_T1L()  NOP16(); NOP2();




void MicroBitAccessibleDisplay::clear()
{
    memset(buffer, 0, num_leds * 3);
}

void MicroBitAccessibleDisplay::update()
{
    NRF_GPIO->OUTCLR = (1UL << pin.name);
    nrf_gpio_cfg_output(pin.name);
    nrf_gpio_pin_clear(pin.name);

    NRF_GPIO_Type * const port = port_from_pin(pin.name);
    const uint32_t pinbit = bit_from_pin(pin.name);
 
    /* Cache the SET / CLR register addresses for speed */
    volatile uint32_t * const SET = &port->OUTSET;
    volatile uint32_t * const CLR = &port->OUTCLR;
 
    // Disable interrupts — critical section start
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    __DSB();

    // Initial start pulse of 50+ us to reset the strip; timing critical but no CPU cycles wasted here.
    target_wait_us(60);

    for (size_t i = 0; i < num_leds; i++) {
        // WS2812 wire order: G7..G0, R7..R0, B7..B0. We send MSB first for each byte.
        uint8_t bytes[3] = {
            buffer[i * 3 + 0],
            buffer[i * 3 + 1],
            buffer[i * 3 + 2],
        };
 
        for (int b = 0; b < 3; b++) {
            uint8_t byte = bytes[b];
 
            #define SEND_BIT(mask)                                          \
                do {                                                        \
                    *SET = pinbit;              /* pin HIGH        */       \
                    if ((byte & (mask))) {                                  \
                        DELAY_T1H();            /* wait T1H window */       \
                        *CLR = pinbit;          /* pin LOW         */       \
                        DELAY_T1L();            /* wait T1L window */       \
                    } else {                                                \
                        DELAY_T0H();            /* wait T0H window */       \
                        *CLR = pinbit;          /* pin LOW         */       \
                        DELAY_T0L();            /* wait T0L window */       \
                    }                                                       \
                } while (0)
 
            SEND_BIT(0x80);
            SEND_BIT(0x40);
            SEND_BIT(0x20);
            SEND_BIT(0x10);
            SEND_BIT(0x08);
            SEND_BIT(0x04);
            SEND_BIT(0x02);
            SEND_BIT(0x01);
 
            #undef SEND_BIT
        }
    }
 
    // Restore interrupt state
    __DSB();
    if (!primask) {
        __enable_irq();
    }
 
    nrf_gpio_pin_set(pin.name);
 }

void MicroBitAccessibleDisplay::set_pixel(uint16_t index, uint8_t value)
{
	if (index >= num_leds)
		return;

	//base_green = 0;
	
	// WS2812B wire order is Green, Red, Blue. We store in this order to avoid having to reorder when we send.
	buffer[index * 3 + 0] = (base_green / 255.0) * value;
	buffer[index * 3 + 1] = (base_red / 255.0) * value;
	buffer[index * 3 + 2] = (base_blue / 255.0) * value;
}

void MicroBitAccessibleDisplay::set_color(uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
{
	if (index >= num_leds)
		return;
	
    // WS2812B wire order is Green, Red, Blue. We store in this order to avoid having to reorder when we send.
    buffer[index * 3 + 0] = green;
    buffer[index * 3 + 1] = red;
    buffer[index * 3 + 2] = blue;
}

void MicroBitAccessibleDisplay::set_color_base(uint8_t red, uint8_t green, uint8_t blue)
{
	base_red = red;
	base_green = green;
	base_blue = blue;
}
