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
#include "FSCache.h"
#include "CodalDmesg.h"

using namespace codal;

/**
 * Create a new instanece of a FileSystem Write Through Cache
 *
 * @param nvm non-volatile memory controller to use as backing store
 * @param blockSize - the size of a logical block, in bytes (n.b. this may be smaller than the physical page size)
 * @param size the maximum number of pages in the cache
 */
FSCache::FSCache(NVMController &nvm, int blockSize, int size) : flash(nvm), blockSize(blockSize), cacheSize(size)
{
	// Initialise space to hold our cached pages.
	cache = (CacheEntry *) malloc(sizeof(CacheEntry)*size);
	memset(cache, 0, sizeof(CacheEntry)*size);

	// Reset operation counter (used for least-recently-used cache replacement policy)
	operationCount = 0;
}

/**
* Clear all cache entries, and free any allocated RAM.
*/
void FSCache::clear()
{
	for (int i = 0; i < cacheSize; i++)
	{
		if (cache[i].page != NULL)
			free(cache[i].page);
	}

	// reset all state.
	memset(cache, 0, sizeof(CacheEntry)*cacheSize);

	// Reset operation counter (used for least-recently-used cache replacement policy)
	operationCount = 0;

}

/**
* Erase a single block at the given address in CACHE memory only, (assuming it is loaded into cache)
*/
int FSCache::erase(uint32_t address)
{
	CacheEntry *c = getCacheEntry(address);

	// Erase the page in our cache (if it is present)
	if (c != NULL)
	{
		memset(c->page, 0xFF, blockSize);
		c->lastUsed = ++operationCount;
	}

	return DEVICE_OK;
}

/**
* Read the given area of memory into the buffer provided,
* paging the data in from FLASH as needed.
*/
int FSCache::read(uint32_t address, const void *data, int len)
{
	int bytesCopied = 0;

	// Ensure that the operation is within the limits of the device
	if (address < flash.getFlashStart() || address + len > flash.getFlashEnd())
		return DEVICE_INVALID_PARAMETER;

	// Read operation may span multiple cache boundaries... so we iterate over blocks as necessary.
	while (bytesCopied < len)
	{
		uint32_t a = address + bytesCopied;
		uint32_t block = (a / blockSize) *blockSize;
		uint32_t offset = a % blockSize;
		uint32_t l = min(len - bytesCopied, blockSize - offset);
		CacheEntry *c = cachePage(block);

		memcpy((uint8_t *)data + bytesCopied, c->page + offset, l);
		bytesCopied += l;
	}

	return DEVICE_OK;
}

/**
* Write the given area of memory into the buffer provided, paging the data in from FLASH as needed.
* Also performs a write-through cache operation directly back if possible. 
* @param address The logical address of the non-volatile storage to write to. DOES NOT need to be word aligned.
* @param data the data to write.
* @param len amount of data to write, in bytes.
* @return DEVICE_OK on success, or DEVICE_NOT_SUPPORTED if the attempted operation is not possible without an ERASE operation.
*/
int FSCache::write(uint32_t address, const void *data, int len)
{
	int bytesCopied = 0;

	// Ensure that the operation is within the limits of the device
	if (address < flash.getFlashStart() || address + len > flash.getFlashEnd())
		return DEVICE_INVALID_PARAMETER;

#ifdef CODAL_FS_CACHE_VALIDATE
	// Read operation may span multiple cache boundaries... so we iterate over blocks as necessary.
	while (bytesCopied < len)
	{
		uint32_t a = address + bytesCopied;
		uint32_t block = (a / blockSize) *blockSize;
		uint32_t offset = a % blockSize;
		uint32_t l = min(len - bytesCopied, blockSize - offset);
		CacheEntry *c = cachePage(block);

		// Validate that a write operation can be performed without needing an erase cycle.
		for (uint32_t i = 0; i < l; i++)
		{
			uint8_t b1 = c->page[offset + i];
			uint8_t b2 = ((uint8_t *)data)[bytesCopied + i];

			if ((b1 ^ b2) & b2)
			{
				DMESG("FS_CACHE: ILLEGAL WRITE OPERAITON ATTEMPTED [ADDRESS: %p] [LENGTH: %d]\n", address, len);
				return DEVICE_NOT_SUPPORTED;
			}
		}

		bytesCopied += l;
	}

#endif

	// Write operation is valid. Update cache and perform a write-through operation to FLASH.
	bytesCopied = 0;
	while (bytesCopied < len)
	{
		uint32_t a = address + bytesCopied;
		uint32_t block = (a / blockSize) *blockSize;
		uint32_t offset = a % blockSize;
		uint32_t l = min(len - bytesCopied, blockSize - offset);
		CacheEntry *c = cachePage(block);

		uint32_t alignedStart = a & 0xFFFFFFFC;
		uint32_t alignedEnd = (a + l) & 0xFFFFFFFC;
		if ((a + l) & 0x03)
			alignedEnd += 4;

		// update cache.
		memcpy(c->page + offset, (uint8_t *)data + bytesCopied, l);

		// Write through (maintaining 32-bit aligned operations)
		flash.write(alignedStart, (uint32_t *)(c->page + (alignedStart % blockSize)), (alignedEnd - alignedStart)/4);

		// Move to next page
		bytesCopied += l;
	}

	return DEVICE_OK;
}

/**
* Pin the given page into cache space.
*/
int FSCache::pin(uint32_t address)
{
	CacheEntry *c = cachePage(address);

	if (c)
	{
		c->flags |= FSCACHE_FLAG_PINNED;
		return DEVICE_OK;
	}

	return DEVICE_NOT_SUPPORTED;
}

/**
* Unpin the given page in the cache.
*/
int FSCache::unpin(uint32_t address)
{
	CacheEntry *c = getCacheEntry(address);

	if (c)
		c->flags &= ~FSCACHE_FLAG_PINNED;

	return DEVICE_OK;
}

/**
* Page a given block into the cache, replacing the LRU block if necessary.
* @param address the logical address of the block to cache.
*/
CacheEntry* FSCache::cachePage(uint32_t address)
{
	CacheEntry *lru = NULL;

	// Ensure the page is not already in the cache. If so, then nothing to do...
	lru = getCacheEntry(address);
	if (lru)
		return lru;

	// Determine the LRU block to replace, or prefereably unused block.
	lru = &cache[0];
	for (int i = 0; i < cacheSize; i++)
	{
		// Simply return the first empty block we find
		if (cache[i].page == NULL)
		{
			lru = &cache[i];
			break;
		}

		// Alternatively, record the least recently used block
		if (!(cache[i].flags & FSCACHE_FLAG_PINNED) && (operationCount - cache[i].lastUsed > operationCount - lru->lastUsed))
			lru = &cache[i];
	}

	// We now have the best block to replace. Update metadata and load in the block from storage.
	// We are a write through cache, so all old values are soft state.
	lru->address = address;
	lru->flags = 0;
	lru->lastUsed = ++operationCount;
	if (lru->page == NULL)
		lru->page = (uint8_t *) malloc(blockSize);

	flash.read((uint32_t *)lru->page, address, blockSize / 4);

	return lru;
}

/**
* Retrieves a given block from the cache, if it is present.
* @param address the logical address of the block.
* @return a pointer to relevent cache entry, or NULL if the block is not cached.
*/
CacheEntry *FSCache::getCacheEntry(uint32_t address)
{
	for (int i = 0; i < cacheSize; i++)
	{
		if (cache[i].address == address && cache[i].page)
		{
			cache[i].lastUsed = ++operationCount;
			return &cache[i];
		}
	}

	return NULL;
}

void FSCache::debug(bool verbose)
{
	for (int i = 0; i < cacheSize; i++)
		debug(&cache[i], verbose);
}

void FSCache::debug(CacheEntry *c, bool verbose)
{
	DMESG("CacheEntry: [address: %p] [lastUsed: %d] [flags: %X]\n", c->address, c->lastUsed, c->flags);

	if (verbose)
	{
		int i = 0;
		int lineLength = 32;
		uint8_t *p = (uint8_t *)c->page;
		uint8_t *end = p + blockSize;

		while (p < end)
		{
			DMESGN("%x ", *p);
			p++;
			i++;
			if (i == lineLength)
			{
				DMESGN("\n");
				i = 0;
			}
		}

		DMESGN("\n\n");
	}
}