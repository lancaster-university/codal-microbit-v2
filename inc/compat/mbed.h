/*
 * Polyfills and warnings for extensions that use mbed classes
 */

#include <stdint.h>
#include <cstdio>
#include <vector>
#include <map>

#include "MicroBitIO.h"
#include "MbedTimer.h"
#include "Ticker.h"
#include "Timeout.h"
#include "InterruptIn.h"
#include "PwmOut.h"
#include "DigitalIn.h"
#include "DigitalOut.h"

using std::vector;
using std::map;

// Missing in codal-core::Pin
#define PIN_CAPABILITY_STANDARD (codal::PinCapability)(0x4 | 0x2 | 0x1)
