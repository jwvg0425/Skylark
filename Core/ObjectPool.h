#pragma once

#include "Exception.h"
#include "Lock.h"

namespace skylark
{
template<class T>
class ObjectPool
{
public:
	enum
	{
		POOL_MAX_SIZE = 4096, // must be power of 2
		POOL_SIZE_MASK = POOL_MAX_SIZE - 1
	};

	//memory pre allocation is just optional
	static void prepareAllocation()
	{
		for (int i = 0; i < POOL_MAX_SIZE; ++i)
		{
			mPool[i] = _aligned_malloc(sizeof(T), MEMORY_ALLOCATION_ALIGNMENT);

			InterlockedAdd(&mTailPos, POOL_MAX_SIZE);
		}
	}

	static void* operator new(size_t objSize)
	{
		long long popPos = InterlockedIncrement64(&mHeadPos) - 1;

		void* popVal = InterlockedExchangePointer(&mPool[popPos & POOL_SIZE_MASK], nullptr);

		if (popVal != nullptr)
		{
			return popVal;
		}

		return _aligned_malloc(objSize, MEMORY_ALLOCATION_ALIGNMENT);
	}

	static void operator delete(void* obj)
	{
		long long insPos = InterlockedIncrement64(&mTailPos) - 1;

		void* prevVal = InterlockedExchangePointer(&mPool[insPos & POOL_SIZE_MASK], obj);

		if (prevVal != nullptr)
		{
			_aligned_free(prevVal);
		}
	}

private:

	static void* volatile mPool[POOL_MAX_SIZE];
	static long long volatile mHeadPos;
	static long long volatile mTailPos;

	static_assert((POOL_MAX_SIZE & POOL_SIZE_MASK) == 0x0, "pool's size must be power of 2");
};

template <typename T>
void* volatile ObjectPool<T>::mPool[POOL_MAX_SIZE] = {};

template <typename T>
long long volatile ObjectPool<T>::mHeadPos(0);

template<typename T>
long long volatile ObjectPool<T>::mTailPos(0);
}