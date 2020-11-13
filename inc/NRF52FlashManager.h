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

#ifndef NRF52_INTERNAL_FLASH_MANAGER_H
#define NRF52_INTERNAL_FLASH_MANAGER_H

#include "codal-core/inc/core/CodalComponent.h"
#include "NVMController.h"

namespace codal
{

/**
 * Class definition for MicroBiInternalFlashManager.
 */
class NRF52FlashManager : public NVMController
{
    private:
        uint32_t startAddress;
        uint32_t pageCount;
        uint32_t pageSize;

    public:
        /**
         * Constructor.
         * Create a software abstraction of an NRF52FlashManager.
         *
         * @param startAddress The start of the memory region to use. All logival addresses start at this address.
         * @param pageCount The number of pages to make available for use.
         * @param pageSize The size of a single FLASH page, in bytes.
         *
         */
        NRF52FlashManager(uint32_t startAddress, uint32_t pageCount, uint32_t pageSize);

        /**
         * Reads a block of memory from non-volatile memory into RAM
         * 
         * @param dest The address in RAM in which to store the result of the read operation
         * @param address The logical address in non-voltile memory to read from
         * @param length The number 32-bit words to read.
         */ 
        virtual int read(uint32_t* dest, uint32_t address, uint32_t length) override;

        /**
         * Writes data to the specified location in the USB file staorage area.
         * 
         * @param data a buffer containing the data to write
         * @param address the location to write to
         * @param length the number of 32-bit words to write
         * 
         * @return DEVICE_OK on success, or error code.
         */
        int write(uint32_t address, uint32_t *data, uint32_t length) override;

        /**
         * Erases a given page in non-volatile memory.
         * 
         * @param page The address of the page to erase (logical address of the start of the page).
         */
        virtual int erase(uint32_t page) override;


        /**
         * Determines the logical address of the start of non-volatile memory region
         * 
         * @return The logical address of the first valid logical address in the region of non-volatile memory
         */
        virtual uint32_t getFlashStart() override;

        /**
         * Determines the logical address of the end of the non-volatile memory region
         * 
         * @return The logical address of the first invalid logical address beyond
         * the non-volatile memory.
         */
        virtual uint32_t getFlashEnd() override;

        /**
         * Determines the size of a non-volatile memory page. A page is defined as
         * the block of memory the can be efficiently erased without impacted on
         * other regions of memory.
         * 
         * @return The size of a single page in bytes.
         */
        virtual uint32_t getPageSize() override;

        /**
         * Determines the amount of available storage.
         * 
         * @return the amount of available storage, in bytes.
         */
        virtual uint32_t getFlashSize() override;

        /**
         * Destructor.
         */
        ~NRF52FlashManager();

};

}
#endif
