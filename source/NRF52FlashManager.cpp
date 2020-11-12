/*
The MIT License (MIT)

Copyright (c) 2020 Lancaster University.

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

#include "NRF52FlashManager.h"
#include "Timer.h"
#include "nrf.h"

#ifdef SOFTDEVICE_PRESENT
#include "nrf_sdh_soc.h"
#include "nrf_sdm.h"
#endif

using namespace codal;

/*
 * The underlying Nordic libraries that support BLE do not compile cleanly with the stringent GCC settings we employ
 * If we're compiling under GCC, then we suppress any warnings generated from this code (but not the rest of the DAL)
 * The ARM cc compiler is more tolerant. We don't test __GNUC__ here to detect GCC as ARMCC also typically sets this
 * as a compatability option, but does not support the options used...
 */
extern "C" void btle_set_user_evt_handler(void (*func)(uint32_t));

static volatile bool flash_op_complete = false;

/*
 * When SoftDevice is present,
 * define an observer to receive system events.
 * Events are delivered here via nrf_sdh and nrf_sdh_soc
 * See NRF_SDH_DISPATCH_MODEL
 */
#ifdef SOFTDEVICE_PRESENT

static void nvmc_event_handler(uint32_t sys_evt, void *)
{
    if (sys_evt == NRF_EVT_FLASH_OPERATION_SUCCESS)
        flash_op_complete = true;
}

NRF_SDH_SOC_OBSERVER( nrf52flash_soc_observer, 0, nvmc_event_handler, NULL);

#endif


/**
 * Constructor.
 * Create a software abstraction of an NRF52FlashManager.
 *
 * @param startAddress The start of the memory region to use. All logival addresses start at this address.
 * @param pageCount The number of pages to make available for use.
 * @param pageSize The size of a single FLASH page, in bytes.
 *
 */
NRF52FlashManager::NRF52FlashManager(uint32_t startAddress, uint32_t pageCount, uint32_t pageSize)
{
    this->startAddress = startAddress;
    this->pageCount = pageCount;
    this->pageSize = pageSize;
}

/**
 * Reads a block of memory from non-volatile memory into RAM
 * 
 * @param dest The address in RAM in which to store the result of the read operation
 * @param address The logical address in non-voltile memory to read from
 * @param length The number 32-bit words to read.
 */ 
int
NRF52FlashManager::read(uint32_t* dest, uint32_t address, uint32_t length)
{   
    uint32_t *source = (uint32_t *) (startAddress + (uint32_t)address);
    memcpy(dest, source, length * sizeof(uint32_t));
    
    return DEVICE_OK;
}

/**
 * Writes data to the specified location in the USB file staorage area.
 * 
 * @param data a buffer containing the data to write
 * @param address the location to write to
 * @param length the number of 32-bit words to write
 * 
 * @return DEVICE_OK on success, or error code.
 */
int 
NRF52FlashManager::write(uint32_t address, uint32_t *data, uint32_t length)
{
    address += startAddress;

#ifdef SOFTDEVICE_PRESENT
    // Schedule SoftDevice to write this memory for us, and wait for it to complete.
    // This happens ASYNCHRONOUSLY when SD is enabled (and synchronously if disabled!!)
    uint8_t sd_enabled = 0;
    sd_softdevice_is_enabled(&sd_enabled);

    if (sd_enabled)
    {
        flash_op_complete = false;

        while(1)
        {
            if (sd_flash_write((uint32_t *) address, data, length) == NRF_SUCCESS)
                break;

            system_timer_wait_ms(10);
        }

        // Wait for SoftDevice to diable the write operation when it completes...
        while(!flash_op_complete);
    }
    else
#endif
    {
        uint32_t *addr = (uint32_t *) address;

        // Turn on flash write enable and wait until the NVMC is ready:
        NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

        for(uint32_t i=0;i<length;i++)
        {
            *(addr+i) = *(data+i);
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
        }

        // Turn off flash write enable and wait until the NVMC is ready:
        NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
    }
    return DEVICE_OK;
}

/**
 * Erases a given page in non-volatile memory.
 * 
 * @param page The address of the page to erase (logical address of the start of the page).
 */
int
NRF52FlashManager::erase(uint32_t page)
{
    page += startAddress;

#ifdef SOFTDEVICE_PRESENT
    uint8_t sd_enabled = 0;
    sd_softdevice_is_enabled(&sd_enabled);
    
    if (sd_enabled)
    {
        flash_op_complete = false;
        while(1)
        {
            if (sd_flash_page_erase(page / pageSize) == NRF_SUCCESS)
                break;

            system_timer_wait_ms(10);
        }
        // Wait for SoftDevice to diable the write operation when it completes...
        while(!flash_op_complete);
    }
    else   
#endif
    {
        // Turn on flash erase enable and wait until the NVMC is ready:
        NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
        
        // Erase page:
        NRF_NVMC->ERASEPAGE = page;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

        // Turn off flash erase enable and wait until the NVMC is ready:
        NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
    }
    return DEVICE_OK;
}


/**
 * Determines the logical address of the start of non-volatile memory region
 * 
 * @return The logical address of the first valid logical address in the region of non-volatile memory
 */
uint32_t
NRF52FlashManager::getFlashStart()
{
    return 0;
}

/**
 * Determines the logical address of the end of the non-volatile memory region
 * 
 * @return The logical address of the first invalid logical address beyond
 * the non-volatile memory.
 */
uint32_t
NRF52FlashManager::getFlashEnd()
{
    return getFlashSize();
}

/**
 * Determines the size of a non-volatile memory page. A page is defined as
 * the block of memory the can be efficiently erased without impacted on
 * other regions of memory.
 * 
 * @return The size of a single page in bytes.
 */
uint32_t
NRF52FlashManager::getPageSize()
{
    return pageSize;
}

/**
 * Determines the amount of available storage.
 * 
 * @return the amount of available storage, in bytes.
 */
uint32_t
NRF52FlashManager::getFlashSize()
{
    return pageCount * pageSize;
}

/**
 * Destructor.
 */
NRF52FlashManager::~NRF52FlashManager()
{

}