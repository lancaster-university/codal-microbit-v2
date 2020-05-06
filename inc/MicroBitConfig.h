#ifndef MICROBIT_CONFIG_H
#define MICROBIT_CONFIG_H

#include "CodalConfig.h"
#include "yotta_cfg_mappings.h"
#include "MicroBitCompat.h"

/*
 * MicroBit specific build configuration options.
 */

// Defines where in memory persistent data is stored.
#ifndef KEY_VALUE_STORE_PAGE
#define KEY_VALUE_STORE_PAGE	                (PAGE_SIZE * (NRF_FICR->CODESIZE - 17))
#endif

#ifndef BLE_BOND_DATA_PAGE
#define BLE_BOND_DATA_PAGE                      (PAGE_SIZE * (NRF_FICR->CODESIZE - 18))
#endif

#ifndef DEFAULT_SCRATCH_PAGE
#define DEFAULT_SCRATCH_PAGE	                (PAGE_SIZE * (NRF_FICR->CODESIZE - 19))
#endif

// Address of the end of the current program in FLASH memory.
// This is recorded by the C/C++ linker, but the symbol name varies depending on which compiler is used.
#if defined(__arm)
extern uint32_t Image$$ER_IROM1$$RO$$Limit;
#define FLASH_PROGRAM_END (uint32_t) (&Image$$ER_IROM1$$RO$$Limit)
#else
#ifdef __cplusplus
extern "C" {
#endif
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
#define FLASH_PROGRAM_END ( (uint32_t) (&__etext) + ( uint32_t)(&__data_end__) - ( uint32_t) (&__data_start__))
#ifdef __cplusplus
}
#endif
#endif

// Enable/Disable BLE during normal operation.
// Set '1' to enable.
#ifndef MICROBIT_BLE_ENABLED
#define MICROBIT_BLE_ENABLED                    0
#endif

// Enable/Disable BLE pairing mode mode at power up.
// Set '1' to enable.
#ifndef MICROBIT_BLE_PAIRING_MODE
#define MICROBIT_BLE_PAIRING_MODE               1
#endif

// Enable/Disable the use of private resolvable addresses.
// Set '1' to enable.
// n.b. This is known to be a feature that suffers compatibility issues with many BLE central devices.
#ifndef MICROBIT_BLE_PRIVATE_ADDRESSES
#define MICROBIT_BLE_PRIVATE_ADDRESSES          0
#endif

// Convenience option to enable / disable BLE security entirely
// Open BLE links are not secure, but commonly used during the development of BLE services
// Set '1' to disable all secuity
#ifndef MICROBIT_BLE_OPEN
#define MICROBIT_BLE_OPEN                       0
#endif

// Configure for open BLE operation if so configured
#if (MICROBIT_BLE_OPEN == 1)
#define MICROBIT_BLE_SECURITY_LEVEL             SECURITY_MODE_ENCRYPTION_OPEN_LINK
#define MICROBIT_BLE_WHITELIST                  0
#define MICROBIT_BLE_ADVERTISING_TIMEOUT        0
#define MICROBIT_BLE_DEFAULT_TX_POWER           6
#endif


// Define the default, global BLE security requirements for MicroBit BLE services
// May be one of the following options (see mbed's SecurityManager class implementaiton detail)
// SECURITY_MODE_ENCRYPTION_OPEN_LINK:      No bonding, encryption, or whitelisting required.
// SECURITY_MODE_ENCRYPTION_NO_MITM:        Bonding, encyption and whitelisting but no passkey.
// SECURITY_MODE_ENCRYPTION_WITH_MITM:      Bonding, encrytion and whitelisting with passkey authentication.
//
#ifndef MICROBIT_BLE_SECURITY_LEVEL
#define MICROBIT_BLE_SECURITY_LEVEL             SECURITY_MODE_ENCRYPTION_WITH_MITM
#endif

// Enable/Disable the use of BLE whitelisting.
// If enabled, the micro:bit will only respond to connection requests from
// known, bonded devices.
#ifndef MICROBIT_BLE_WHITELIST
#define MICROBIT_BLE_WHITELIST                  1
#endif

// Define the period of time for which the BLE stack will advertise (seconds)
// Afer this period, advertising will cease. Set to '0' for no timeout (always advertise).
#ifndef MICROBIT_BLE_ADVERTISING_TIMEOUT
#define MICROBIT_BLE_ADVERTISING_TIMEOUT        0
#endif

// Define the default BLE advertising interval in ms
#ifndef MICROBIT_BLE_ADVERTISING_INTERVAL
#define MICROBIT_BLE_ADVERTISING_INTERVAL        50
#endif

// Defines default power level of the BLE radio transmitter.
// Valid values are in the range 0..7 inclusive, with 0 being the lowest power and 7 the highest power.
// Based on trials undertaken by the BBC, the radio is normally set to its lowest power level
// to best protect children's privacy.
#ifndef MICROBIT_BLE_DEFAULT_TX_POWER
#define MICROBIT_BLE_DEFAULT_TX_POWER           0
#endif

// Enable/Disable BLE Service: MicroBitDFU
// This allows over the air programming during normal operation.
// Set '1' to enable.
#ifndef MICROBIT_BLE_DFU_SERVICE
#define MICROBIT_BLE_DFU_SERVICE                1
#endif

// Enable/Disable BLE Service: MicroBitDeviceInformationService
// This enables the standard BLE device information service.
// Set '1' to enable.
#ifndef MICROBIT_BLE_DEVICE_INFORMATION_SERVICE
#define MICROBIT_BLE_DEVICE_INFORMATION_SERVICE 1
#endif

// Versioning options.
// We use semantic versioning (http://semver.org/) to identify differnet versions of the micro:bit runtime.
// Where possible we use yotta (an ARM mbed build tool) to help us track versions.
// if this isn't available, it can be defined manually as a configuration option.
//
#ifndef MICROBIT_DAL_VERSION
#define MICROBIT_DAL_VERSION                    "unknown"
#endif

#define PAGE_SIZE 4096

#endif
