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
				pool[i] = _aligned_malloc(sizeof(T), MEMORY_ALLOCATION_ALIGNMENT);

				InterlockedAdd(&tailPos, POOL_MAX_SIZE);
			}
		}

		static void* operator new(size_t objSize)
		{
			long long popPos = InterlockedIncrement64(&headPos) - 1;

			void* popVal = InterlockedExchangePointer(&pool[popPos & POOL_SIZE_MASK], nullptr);

			if (popVal != nullptr)
			{
				return popVal;
			}

			return _aligned_malloc(objSize, MEMORY_ALLOCATION_ALIGNMENT);
		}

		static void operator delete(void* obj)
		{
			long long insPos = InterlockedIncrement64(&tailPos) - 1;

			void* prevVal = InterlockedExchangePointer(&pool[insPos & POOL_SIZE_MASK], obj);

			if (prevVal != nullptr)
			{
				_aligned_free(prevVal);
			}
		}

	private:
		
		static void* volatile pool[POOL_MAX_SIZE];
		static long long volatile headPos;
		static long long volatile tailPos;

		void* __powerof2check__[((POOL_MAX_SIZE & POOL_SIZE_MASK) == 0x0) & 0x1];
	};

	template <typename T>
	void* volatile ObjectPool<T>::pool[POOL_MAX_SIZE] = {};

	template <typename T>
	long long volatile ObjectPool<T>::headPos(0);

	template<typename T>
	long long volatile ObjectPool<T>::tailPos(0);
}