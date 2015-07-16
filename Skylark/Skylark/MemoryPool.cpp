#include "stdafx.h"
#include "MemoryPool.h"
#include "Exception.h"

skylark::MemoryPool* skylark::MemoryPool::pool = nullptr;

skylark::SmallSizeMemoryPool::SmallSizeMemoryPool(DWORD allocSize_)
	: allocSize(allocSize_)
{
	CRASH_ASSERT(allocSize > MEMORY_ALLOCATION_ALIGNMENT);
	InitializeSListHead(&freeList);
}

skylark::MemAllocInfo* skylark::SmallSizeMemoryPool::pop()
{
	MemAllocInfo* mem = (MemAllocInfo*)InterlockedPopEntrySList(&freeList);
	if (NULL == mem)
	{
		mem = reinterpret_cast<MemAllocInfo*>(_aligned_malloc(allocSize, MEMORY_ALLOCATION_ALIGNMENT));
	}
	else
	{
		CRASH_ASSERT(mem->allocSize == 0);
	}

	InterlockedIncrement(&allocCount);
	return mem;
}

void skylark::SmallSizeMemoryPool::push(MemAllocInfo* ptr)
{
	InterlockedPushEntrySList(&freeList, (PSLIST_ENTRY)ptr);
	InterlockedDecrement(&allocCount);
}

skylark::MemoryPool* skylark::MemoryPool::GetInstance()
{
	if (pool == nullptr)
	{
		pool = new skylark::MemoryPool();
	}

	return pool;
}

void* skylark::MemoryPool::allocate(unsigned int size)
{
	MemAllocInfo* header = nullptr;
	unsigned int realAllocSize = size + sizeof(MemAllocInfo);

	if (realAllocSize > MAX_CHUNK_SIZE)
	{
		header = reinterpret_cast<MemAllocInfo*>(_aligned_malloc(realAllocSize, MEMORY_ALLOCATION_ALIGNMENT));
	}
	else
	{
		header = smallSizeMemoryPoolTable[realAllocSize]->pop();
	}

	return attachMemAllocInfo(header, realAllocSize);
}

void skylark::MemoryPool::deallocate(void* ptr, long extraInfo)
{
	MemAllocInfo* header = detachMemAllocInfo(ptr);
	header->extraInfo = extraInfo; // info about recent memory allocation

	long realAllocSize = InterlockedExchange(&header->allocSize, 0);

	CRASH_ASSERT(realAllocSize > 0);

	if (realAllocSize > MAX_CHUNK_SIZE)
	{
		_aligned_free(header);
	}
	else
	{
		smallSizeMemoryPoolTable[realAllocSize]->push(header);
	}
}

skylark::MemoryPool::MemoryPool()
{
	memset(smallSizeMemoryPoolTable, 0, sizeof(smallSizeMemoryPoolTable));

	int currIdx = 1;
	for (int i = 0; i <= MAX_POOL_COUNT; ++i)
	{
		int chunkSize = ((i % 4) + 4) << (1 + (i >> 2));

		if (chunkSize > MEMORY_ALLOCATION_ALIGNMENT && chunkSize % MEMORY_ALLOCATION_ALIGNMENT == 0)
		{
			auto newPool = new SmallSizeMemoryPool(chunkSize);
			while (currIdx <= chunkSize)
			{
				smallSizeMemoryPoolTable[currIdx++] = newPool;
			}
		}
	}
}
