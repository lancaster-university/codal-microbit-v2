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

using std::vector;
using std::map;
