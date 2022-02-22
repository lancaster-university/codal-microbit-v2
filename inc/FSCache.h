/*
The MIT License (MIT)
Copyright (c) 2021 Lancaster University.
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

#ifndef FSCache_H
#define FSCache_H

#include "NVMController.h"
#include "CodalCompat.h"

#define FSCACHE_FLAG_PINNED				0x01

#define CODAL_FS_CACHE_VALIDATE			1
#define CODAL_FS_DEFAULT_CACHE_SZE		4

namespace codal
{
	struct CacheEntry
	{
		uint32_t address;
		uint16_t lastUsed;
		uint16_t flags;
		uint8_t  *page;
	};

	class FSCache
	{
		private:
			NVMController &flash;
			CacheEntry* cache;
			int blockSize;
			int cacheSize;
			uint16_t operationCount;

		public:
		  /**
			* @param nvm non - volatile memory controller to use as backing store
			* @param blockSize - the size of a logical block, in bytes (n.b. this may be smaller than the physical page size)
			* @param size the maximum number of pages in the cache
			*/
			FSCache(NVMController &nvm, int blockSize, int size = CODAL_FS_DEFAULT_CACHE_SZE);

			/**
			 * Clear all cache entries, and free any allocated RAM.
			 */
			void clear();

			/**
			 * Erase a single page of FLASH memory at the given address
			 */
			int erase(uint32_t address);

			/**
			 * Read the given area of memory into the buffer provided,
			 * paging the data in from FLASH as needed.
			 */
			int read(uint32_t address, const void *data, int len);

			/**
			* Write the given area of memory into the buffer provided,
			* paging the data in from FLASH as needed.
			* n.b. data doe NOT need to be word aligned.
			*/
			int write(uint32_t address, const void *data, int len);

			/**
			 * Pin the given page into cache space.
			 */
			int pin(uint32_t address);

			/**
			* Unpin the given page into cache space.
			*/
			int unpin(uint32_t address);

			/**
			 * Retrieves a given block from the cache, if it is present.
			 * @param address the logical address of the block.
			 * @return a pointer to relevent cache entry, or NULL if the block is not cached.
			 */
			CacheEntry *getCacheEntry(uint32_t address);

			/**
			 * Page a given block into the cache, replacing the LRU block if necessary.
			 * @param address the logical address of the block to cache.
			 * @return a pointer to the relevant cache entry.
			 */
			CacheEntry *cachePage(uint32_t address);

			void debug(bool verbose = true);
			void debug(CacheEntry *c, bool verbose = true);
	};
}


#endif
