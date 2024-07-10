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
#include "hal/nrf_gpio.h"
#include "cmsis_compiler.h"

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
  * Perfom scheduler idle
  */
void MicroBitDevice::schedulerIdle()
{
}


/**
  * Seed the pseudo random number generator using the hardware random number generator.
  *
  * @code
  * uBit.seedRandom();
  * @endcode
  */
void MicroBitDevice::seedRandom()
{
    uint32_t r = 0xBBC5EED;

    if(!ble_running())
    {
        // Start the Random number generator. No need to leave it running... I hope. :-)
        NRF_RNG->TASKS_START = 1;

        for(int i = 0; i < 4; i++)
        {
            // Clear the VALRDY EVENT
            NRF_RNG->EVENTS_VALRDY = 0;

            // Wait for a number ot be generated.
            while(NRF_RNG->EVENTS_VALRDY == 0);

            r = (r << 8) | ((int) NRF_RNG->VALUE);
        }

        // Disable the generator to save power.
        NRF_RNG->TASKS_STOP = 1;
    }

    seedRandom(r);
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
    return CodalDevice::seedRandom(seed);
}


int microbit_random(int max)
{
    return microbit_device_instance ? microbit_device_instance->random(max) : 0;
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
__NO_RETURN void microbit_reset()
{
    NVIC_SystemReset();

    // __NO_RETURN added to NVIC_SystemReset() in CMSIS V5.0.5
    // Currently using V5.0.3, so this can be removed if updated in the future
    // Looks like it gets compiled out anyway
    for (;;);
}

/**
  * For DAL compatibility, determine the version of DAL/CODAL currently running.
  * @return a pointer to a character buffer containing a representation of the semantic version number.
  */
const char * microbit_dal_version()
{
    return MICROBIT_DAL_VERSION;
}

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
    if(microbit_device_instance)
        microbit_device_instance->seedRandom();
}

/**
  * Seed the pseudo random number generator (RNG) using the given 32-bit value.
  * This function does not use the NRF52833's in built cryptographic random number generator.
  *
  * @param seed The value to use as a seed.
  */
void microbit_seed_random(uint32_t seed)
{
    if(microbit_device_instance)
        microbit_device_instance->seedRandom(seed);
}


// LED matrix map for use by microbit_panic

typedef uint8_t microbit_LEDMapPinNumber;

typedef int (*microbit_LEDMapFnRCtoXY)( int row, int col);

typedef struct microbit_LEDMapStr
{
    int         width;                      // The physical width of the LED matrix, in pixels.
    int         height;                     // The physical height of the LED matrix, in pixels.
    int         rows;                       // The number of drive pins connected to LEDs.
    int         columns;                    // The number of sink pins connected to the LEDs.

    const microbit_LEDMapPinNumber *rowPins;      // Array of pin numbers to drive.
    const microbit_LEDMapPinNumber *columnPins;   // Array of pin numbers to sink.
    
    microbit_LEDMapFnRCtoXY mapRCtoX;       // Map logical LED positions to physical positions
    microbit_LEDMapFnRCtoXY mapRCtoY;
} microbit_LEDMapStr;


// LED matrix map values for micro:bit V2

// Width and height of LEDs
#ifndef microbit_LEDMap_WIDTH
#define microbit_LEDMap_WIDTH 5
#endif

#ifndef microbit_LEDMap_HEIGHT
#define microbit_LEDMap_HEIGHT 5
#endif

// Width and height of LED pins
#ifndef microbit_LEDMap_COLS
#define microbit_LEDMap_COLS 5
#endif

#ifndef microbit_LEDMap_ROWS
#define microbit_LEDMap_ROWS 5
#endif

// LED rows pinmap
#ifndef microbit_LEDMap_PINROW0
#define microbit_LEDMap_PINROW0 21
#endif

#ifndef microbit_LEDMap_PINROW1
#define microbit_LEDMap_PINROW1 22
#endif

#ifndef microbit_LEDMap_PINROW2
#define microbit_LEDMap_PINROW2 15
#endif

#ifndef microbit_LEDMap_PINROW3
#define microbit_LEDMap_PINROW3 24
#endif

#ifndef microbit_LEDMap_PINROW4
#define microbit_LEDMap_PINROW4 19
#endif

// LED columns pinmap
#ifndef microbit_LEDMap_PINCOL0
#define microbit_LEDMap_PINCOL0 28
#endif

#ifndef microbit_LEDMap_PINCOL1
#define microbit_LEDMap_PINCOL1 11
#endif

#ifndef microbit_LEDMap_PINCOL2
#define microbit_LEDMap_PINCOL2 31
#endif

#ifndef microbit_LEDMap_PINCOL3
#define microbit_LEDMap_PINCOL3 37
#endif

#ifndef microbit_LEDMap_PINCOL4
#define microbit_LEDMap_PINCOL4 30
#endif

static const microbit_LEDMapPinNumber microbit_LEDMap_rowPins[ microbit_LEDMap_ROWS] =
{
    microbit_LEDMap_PINROW0, microbit_LEDMap_PINROW1, microbit_LEDMap_PINROW2, microbit_LEDMap_PINROW3, microbit_LEDMap_PINROW4
};

static const microbit_LEDMapPinNumber microbit_LEDMap_columnPins[ microbit_LEDMap_COLS] =
{
    microbit_LEDMap_PINCOL0, microbit_LEDMap_PINCOL1, microbit_LEDMap_PINCOL2, microbit_LEDMap_PINCOL3, microbit_LEDMap_PINCOL4
};

static int microbit_LEDMap_RCtoX( int row, int col) { return col; }
static int microbit_LEDMap_RCtoY( int row, int col) { return row; }

static const microbit_LEDMapStr microbit_LEDMap =
{
    microbit_LEDMap_WIDTH, microbit_LEDMap_HEIGHT,
    microbit_LEDMap_COLS, microbit_LEDMap_ROWS,
    microbit_LEDMap_rowPins,
    microbit_LEDMap_columnPins,
    microbit_LEDMap_RCtoX,
    microbit_LEDMap_RCtoY
};


// Low level LEDs pin functions

static inline void microbit_LEDMap_rowOn(        int row)           { nrf_gpio_pin_set(   microbit_LEDMap.rowPins[ row]); }
static inline void microbit_LEDMap_rowOff(       int row)           { nrf_gpio_pin_clear( microbit_LEDMap.rowPins[ row]); }

static inline void microbit_LEDMap_columnOn(     int col, bool on)  { nrf_gpio_pin_write( microbit_LEDMap.columnPins[ col], on ? 0 : 1); }
static inline void microbit_LEDMap_columnOff(    int col)           { nrf_gpio_pin_set(   microbit_LEDMap.columnPins[ col]); }

static inline void microbit_LEDMap_pinOutput(    uint8_t pin)       { nrf_gpio_cfg_output( pin); }

static inline void microbit_LEDMap_configure()
{
    for ( int i = 0; i < microbit_LEDMap.columns; i++)  microbit_LEDMap_pinOutput( microbit_LEDMap.columnPins[i]);
    for ( int i = 0; i < microbit_LEDMap.rows;    i++)  microbit_LEDMap_pinOutput( microbit_LEDMap.rowPins[i]);
}


// Constants for microbit_panic

// length of message: face, code digit, code digit, code digit
#ifndef microbit_panic_MSGLEN
#define microbit_panic_MSGLEN         4
#endif

// position of first code digit
#ifndef microbit_panic_MSG1STDIGIT
#define microbit_panic_MSG1STDIGIT    1
#endif

// divisor for first digit
#ifndef microbit_panic_DIVMAX
#define microbit_panic_DIVMAX         100
#endif

// divisor base
#ifndef microbit_panic_DIVBASE
#define microbit_panic_DIVBASE        10
#endif

// Number of burn passes for each character
#ifndef microbit_panic_SCANS
#define microbit_panic_SCANS         48    //1000 ~ 25s
#endif

// Passes left clear for repeated character
#ifndef microbit_panic_CLEARSCANS
#define microbit_panic_CLEARSCANS    8    //1000 ~ 25s
#endif

// Delay cycles for each row
#ifndef microbit_panic_ROWDELAY
#define microbit_panic_ROWDELAY       (40000)
#endif

static const uint8_t panic_face[ MICROBIT_FONT_WIDTH] = { 0x1B, 0x1B, 0x0, 0x0E, 0x11};

static int panic_timeout = 0;


void microbit_panic_timeout(int iterations)
{
    panic_timeout = iterations;
}

__NO_RETURN void microbit_panic( int statusCode)
{
    const microbit_LEDMapStr &mm = microbit_LEDMap;
    uint8_t chr;
    
    target_disable_irq();
    
    for (int i=0; i<8; i++)
        NRF_GPIOTE->CONFIG[i] = 0;
    
    microbit_LEDMap_configure();
    
    for ( int row = 0; row < mm.rows; row++)
      microbit_LEDMap_rowOff( row);
    
    for ( int repeat = 0; panic_timeout == 0 || repeat < panic_timeout; repeat++)
    {
        for ( int msgIdx = 0; msgIdx < microbit_panic_MSGLEN; msgIdx++)
        {
            // find the the current character and its font bytes
            chr = 0;
            if ( msgIdx >= microbit_panic_MSG1STDIGIT)
            {
                // calculate divisor for this digit: 100s, 10s or units
                int div = microbit_panic_DIVMAX;
                for ( int digit = msgIdx - microbit_panic_MSG1STDIGIT; digit > 0; digit--)
                    div /= microbit_panic_DIVBASE;
                chr = 0x30 + ( statusCode / div) % microbit_panic_DIVBASE;
            }
            const uint8_t *fontBytes = chr ? ( BitmapFont::defaultFont + BITMAP_FONT_WIDTH * ( chr - BITMAP_FONT_ASCII_START)) : panic_face;
            
            for ( int scan = 0; scan < microbit_panic_SCANS; scan++)
            {
                int rowOnCycles = microbit_panic_ROWDELAY;
                if ( scan >= microbit_panic_SCANS - microbit_panic_CLEARSCANS)   // blank display between characters
                    rowOnCycles = 0;
                else if ( repeat >= 5)                                  // dim display after 5 repetitions
                    rowOnCycles = rowOnCycles / 10;
                
                // turn each row on and off
                for ( int row = 0; row < mm.rows; row++)
                {
                    for ( int col = 0; col < mm.columns; col++)
                    {
                        microbit_LEDMap_columnOn( col,
                            fontBytes[ mm.mapRCtoY( row, col)] & ( 1 << ( mm.width - 1 - mm.mapRCtoX( row, col))));
                    }
                        
                    if ( rowOnCycles)
                      microbit_LEDMap_rowOn( row);

                    for ( int i = 0; i < microbit_panic_ROWDELAY; i++)
                    {
                        nrf_gpio_pin_out_read( microbit_LEDMap.columnPins[0]);
                        if ( i == rowOnCycles - 1)
                            microbit_LEDMap_rowOff( row);
                    }
                }
            }
        }
    }
    
    microbit_reset();
}

} // namespace codal


// Override for CODAL target HAL

__attribute__((weak)) void target_panic( int statusCode)
{
    codal::microbit_panic( statusCode);
}

extern "C"
{

__attribute__((weak)) int __wrap_atexit(void (*function)(void))
{
    return -1;
}

} // extern "C"

__attribute__((weak)) void target_scheduler_idle()
{
    if ( microbit_device_instance)
        microbit_device_instance->schedulerIdle();
    else
        target_wait_for_event();
}
