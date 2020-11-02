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

#define PullUp PullMode::Up
#define PullDown PullMode::Down
#define NoPull PullMode::None

#define DEVICE_ID_IO_MBED_INTERRUPT_IN 0xE0
#define DEVICE_ID_IO_MBED_PWM          0xE1

using std::vector;
using std::map;
