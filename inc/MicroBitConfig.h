#ifndef MICROBIT_CONFIG_H
#define MICROBIT_CONFIG_H

#include "CodalConfig.h"
#include "yotta_cfg_mappings.h"
#include "MicroBitCompat.h"

#ifdef SOFTDEVICE_PRESENT
#include "nrf_mbr.h"
#include "nrf_sdm.h"
#endif

/*
 * MicroBit specific build configuration options.
 */

#ifndef MICROBIT_CODEPAGESIZE
#define MICROBIT_CODEPAGESIZE           ((uint32_t) NRF_FICR->CODEPAGESIZE)
#endif

#ifndef MICROBIT_CODESIZE
#define MICROBIT_CODESIZE               ((uint32_t) NRF_FICR->CODESIZE)
#endif

#ifndef MICROBIT_BOOTLOADER_ADDRESS
#ifdef BOOTLOADER_ADDRESS
#define MICROBIT_BOOTLOADER_ADDRESS     ( BOOTLOADER_ADDRESS)
#else
#ifdef SOFTDEVICE_PRESENT
#define MICROBIT_BOOTLOADER_ADDRESS     ((*(uint32_t *) MBR_BOOTLOADER_ADDR) != 0xFFFFFFFF ? *(uint32_t *) MBR_BOOTLOADER_ADDR : NRF_UICR->NRFFW[0])
#else
#define MICROBIT_BOOTLOADER_ADDRESS     ( NRF_UICR->NRFFW[0])
#endif
#endif
#endif

#ifndef MICROBIT_BOOTLOADER_SIZE
#define MICROBIT_BOOTLOADER_SIZE        ( MICROBIT_CODESIZE * MICROBIT_CODEPAGESIZE - MICROBIT_BOOTLOADER_ADDRESS)
#endif

#ifndef MICROBIT_MBR_PARAMS
#ifdef MBR_PARAMS_PAGE_ADDRESS
#define MICROBIT_MBR_PARAMS             ( MBR_PARAMS_PAGE_ADDRESS)
#else
#ifdef SOFTDEVICE_PRESENT
#define MICROBIT_MBR_PARAMS             ((*(uint32_t *) MBR_PARAM_PAGE_ADDR) != 0xFFFFFFFF ? *(uint32_t *) MBR_PARAM_PAGE_ADDR : NRF_UICR->NRFFW[1])
#else
#define MICROBIT_MBR_PARAMS             ( NRF_UICR->NRFFW[1])
#endif
#endif
#endif

#ifndef MICROBIT_BOOTLOADER_SETTINGS
#define MICROBIT_BOOTLOADER_SETTINGS    ( MICROBIT_MBR_PARAMS + MICROBIT_CODEPAGESIZE)
#endif

// Defines where in memory persistent data is stored.
#ifndef MICROBIT_STORAGE_PAGE
#define MICROBIT_STORAGE_PAGE           ( MICROBIT_BOOTLOADER_ADDRESS - MICROBIT_CODEPAGESIZE)
#endif

#ifndef MICROBIT_FDS_PAGE
#define MICROBIT_FDS_PAGE               ( MICROBIT_BOOTLOADER_ADDRESS - MICROBIT_CODEPAGESIZE * 3)
#endif

#ifndef MICROBIT_DEFAULT_SCRATCH_PAGE
#define MICROBIT_DEFAULT_SCRATCH_PAGE   ( MICROBIT_BOOTLOADER_ADDRESS - MICROBIT_CODEPAGESIZE * 4)
#endif

#ifndef MICROBIT_STORAGE_SCRATCH_PAGE
#define MICROBIT_STORAGE_SCRATCH_PAGE   ( MICROBIT_DEFAULT_SCRATCH_PAGE)
#endif

#ifndef MICROBIT_SOFTDEVICE_EXISTS

#define MICROBIT_SOFTDEVICE_EXISTS      ( (*(uint32_t *) 0x3004) == ((uint32_t) 0x51B1E5DB) )
#endif

#ifndef MICROBIT_APP_REGION_START
#ifdef SOFTDEVICE_PRESENT
#define MICROBIT_APP_REGION_START       ( MBR_SIZE + SD_FLASH_SIZE)
#else
#define MICROBIT_APP_REGION_START       ( MICROBIT_SOFTDEVICE_EXISTS ? (*(uint32_t *) 0x3008) : (uint32_t)0)
#endif
#endif


#ifndef MICROBIT_APP_REGION_END
#define MICROBIT_APP_REGION_END         ( MICROBIT_DEFAULT_SCRATCH_PAGE)
#endif


#ifndef PAGE_SIZE
#define PAGE_SIZE                       ( MICROBIT_CODEPAGESIZE)
#endif

#ifndef KEY_VALUE_STORE_PAGE
#define KEY_VALUE_STORE_PAGE            ( MICROBIT_STORAGE_PAGE)
#endif

#ifndef BLE_BOND_DATA_PAGE
#define BLE_BOND_DATA_PAGE              ( MICROBIT_FDS_PAGE)
#endif

#ifndef DEFAULT_SCRATCH_PAGE
#define DEFAULT_SCRATCH_PAGE            ( MICROBIT_DEFAULT_SCRATCH_PAGE)
#endif

//
// File System configuration defaults
//

//
// Defines the logical block size for the file system.
// Must be a factor of the physical PAGE_SIZE (ideally a power of two less).
//
#ifndef MBFS_BLOCK_SIZE
#define MBFS_BLOCK_SIZE        256
#endif

//
// FileSystem writeback cache size, in bytes. Defines how many bytes will be stored
// in RAM before being written back to FLASH. Set to zero to disable this feature.
// Should be <= MBFS_BLOCK_SIZE.
//
#ifndef MBFS_CACHE_SIZE
#define MBFS_CACHE_SIZE        0
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
#define MICROBIT_BLE_SECURITY_MODE              1
#define MICROBIT_BLE_WHITELIST                  0
#define MICROBIT_BLE_ADVERTISING_TIMEOUT        0
#define MICROBIT_BLE_DEFAULT_TX_POWER           6
#endif

// Define the default, global BLE security requirements for MicroBit BLE services
// MICROBIT_BLE_SECURITY_MODE is used instead of MICROBIT_BLE_SECURITY_LEVEL
// If MICROBIT_BLE_SECURITY_LEVEL and MICROBIT_BLE_SECURITY_MODE is defined, they must match as follows
// SEC_OPEN         / OPEN_LINK:    1
// SEC_JUST_WORKS   / NO_MITM:      2
// SEC_MITM         / WITH_MITM:    3
//
#ifndef MICROBIT_BLE_SECURITY_MODE
// Calculate MICROBIT_BLE_SECURITY_MODE from MICROBIT_BLE_SECURITY_LEVEL
#define __MICROBIT_CONFIG_SECURITY_MODE_ENCRYPTION_OPEN_LINK 1
#define __MICROBIT_CONFIG_SECURITY_MODE_ENCRYPTION_NO_MITM   2
#define __MICROBIT_CONFIG_SECURITY_MODE_ENCRYPTION_WITH_MITM 3
#define __MICROBIT_CONFIG_CAT(a, ...) a##__VA_ARGS__
#define __MICROBIT_CONFIG_SECURITY_MODE(x) __MICROBIT_CONFIG_CAT(__MICROBIT_CONFIG_, x)
#define __MICROBIT_CONFIG_SECURITY_MODE_IS(x) (__MICROBIT_CONFIG_SECURITY_MODE(MICROBIT_BLE_SECURITY_LEVEL) == __MICROBIT_CONFIG_SECURITY_MODE(x))

#if (__MICROBIT_CONFIG_SECURITY_MODE_IS(SECURITY_MODE_ENCRYPTION_NO_MITM))
#define MICROBIT_BLE_SECURITY_MODE 2
#elif (__MICROBIT_CONFIG_SECURITY_MODE_IS(SECURITY_MODE_ENCRYPTION_OPEN_LINK))
#define MICROBIT_BLE_SECURITY_MODE 1
#elif (__MICROBIT_CONFIG_SECURITY_MODE_IS(SECURITY_MODE_ENCRYPTION_WITH_MITM))
#define MICROBIT_BLE_SECURITY_MODE 3
#else
#define MICROBIT_BLE_SECURITY_MODE 3
#endif
#endif //#ifndef MICROBIT_BLE_SECURITY_MODE

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

#endif
