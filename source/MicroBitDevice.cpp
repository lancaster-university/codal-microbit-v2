/*
The MIT License (MIT)

Copyright (c) 2016 Lancaster University, UK.

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

#include "MicroBitConfig.h"
#include "MicroBitDevice.h"
#include "nrf.h"

#ifdef SOFTDEVICE_PRESENT
#include "nrf_sdm.h"
#endif

using namespace codal;

static char friendly_name[MICROBIT_NAME_LENGTH+1];

// internal reference to any created instance of the class - used purely for convenience functions.
MicroBitDevice *microbit_device_instance = NULL;

/**
  * Constructor.
  */
MicroBitDevice::MicroBitDevice()
{
    microbit_device_instance = this;
}

namespace codal {

/**
  * Seed the pseudo random number generator using the hardware random number generator.
  *
  * @code
  * uBit.seedRandom();
  * @endcode
  */
void MicroBitDevice::seedRandom()
{
    microbit_seed_random();
}

/**
  * Seed the pseudo random number generator using the given value.
  *
  * @param seed The 32-bit value to seed the generator with.
  *
  * @code
  * uBit.seedRandom(0xBB5EED);
  * @endcode
  */
int MicroBitDevice::seedRandom(uint32_t seed)
{
    microbit_seed_random(seed);
    return DEVICE_OK;
}


int microbit_random(int max)
{
    return microbit_device_instance ? microbit_device_instance->random(max) : 0;
}

static void wait_cycles(uint32_t cycles)
{
    __asm__ __volatile__(".syntax unified\n"
                         "1: subs %0, #1\n"
                         "bne 1b\n"
                         : "+r"(cycles)
                         :
                         :);
}

// TODO make this actually display the error code on the screen
// for now it just blinks
void microbit_panic(int statusCode)
{
    target_disable_irq();
    DMESG("*** CODAL PANIC : [%d]", statusCode);
    NRF_P0->OUT = 0;
    NRF_P1->OUT = 0;
    NRF_P0->DIR = 0;
    NRF_P1->DIR = 0;
    NRF_P0->PIN_CNF[21] = 0x303;
    NRF_P1->PIN_CNF[5] = 0x303;
    while (1)
    {
        NRF_P0->OUT = 0xffffffff;
        wait_cycles(1 << 21);
        NRF_P0->OUT = 0;
        wait_cycles(1 << 21);
    }
}

void microbit_panic_timeout(int iterations)
{
    //TODO: Populate this once a graphical panic is implemented
}

/**
 * Derive a unique, consistent serial number of this device from internal data.
 *
 * @return the serial number of this device.
 */
uint32_t microbit_serial_number()
{
    return NRF_FICR->DEVICEID[1];
}

/**
 * Derive the friendly name for this device, based on its serial number.
 *
 * @return the friendly name of this device.
 */
char* microbit_friendly_name()
{
    const uint8_t codebook[MICROBIT_NAME_LENGTH][MICROBIT_NAME_CODE_LETTERS] =
    {
        {'z', 'v', 'g', 'p', 't'},
        {'u', 'o', 'i', 'e', 'a'},
        {'z', 'v', 'g', 'p', 't'},
        {'u', 'o', 'i', 'e', 'a'},
        {'z', 'v', 'g', 'p', 't'}
    };

    // We count right to left, so create a pointer to the end of the buffer.
    char *name = friendly_name;
    name += MICROBIT_NAME_LENGTH;

    // Terminate the string.
    *name = 0;

    // Derive our name from the MicroBit's serial number.
    uint32_t n = microbit_serial_number();
    int ld = 1;
    int d = MICROBIT_NAME_CODE_LETTERS;
    int h;

    for (int i=0; i<MICROBIT_NAME_LENGTH; i++)
    {
        h = (n % d) / ld;
        n -= h;
        d *= MICROBIT_NAME_CODE_LETTERS;
        ld *= MICROBIT_NAME_CODE_LETTERS;
        *--name = codebook[i][h];
    }

    return friendly_name;
}

/**
  * Determines if a BLE stack is currently running.
  *
  * @return true is a bluetooth stack is operational, false otherwise.
  */
bool ble_running()
{
    uint8_t t = 0;

#ifdef SOFTDEVICE_PRESENT
    sd_softdevice_is_enabled(&t);
#endif

    return t==1;
}

/**
  * Perform a hard reset of the micro:bit.
  */
void
microbit_reset()
{
    NVIC_SystemReset();
}

static uint32_t random_value = 0;

/**
  * Seed the random number generator (RNG).
  *
  * This function uses the NRF52833's in built cryptographic random number generator to seed a Galois LFSR.
  * We do this as the hardware RNG is relatively high power, and is locked out by the BLE stack internally,
  * with a less than optimal application interface. A Galois LFSR is sufficient for our
  * applications, and much more lightweight.
  */
void microbit_seed_random()
{
    random_value = 0;

    if(ble_running())
    {
        /* Currently no support for the soft device. */
        random_value = 0xBBC5EED;
    }
    else
    {
        // Othwerwise we can access the hardware RNG directly.

        // Start the Random number generator. No need to leave it running... I hope. :-)
        NRF_RNG->TASKS_START = 1;

        for(int i = 0; i < 4; i++)
        {
            // Clear the VALRDY EVENT
            NRF_RNG->EVENTS_VALRDY = 0;

            // Wait for a number ot be generated.
            while(NRF_RNG->EVENTS_VALRDY == 0);

            random_value = (random_value << 8) | ((int) NRF_RNG->VALUE);
        }

        // Disable the generator to save power.
        NRF_RNG->TASKS_STOP = 1;
    }
}

/**
  * Seed the pseudo random number generator (RNG) using the given 32-bit value.
  * This function does not use the NRF52833's in built cryptographic random number generator.
  *
  * @param seed The value to use as a seed.
  */
void microbit_seed_random(uint32_t seed)
{
    random_value = seed;
}

}
