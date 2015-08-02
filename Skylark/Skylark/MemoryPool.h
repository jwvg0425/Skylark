#pragma once

namespace skylark
{
	__declspec(align(MEMORY_ALLOCATION_ALIGNMENT))
	struct MemAllocInfo : SLIST_ENTRY
	{
		MemAllocInfo(int size) : allocSize(size), extraInfo(-1)
		{}

		long allocSize; // including MemAllocInfo's size
		long extraInfo; // additonal info

	}; //total 16 bytes

	inline void* attachMemAllocInfo(MemAllocInfo* header, int size)
	{
		new (header)MemAllocInfo(size);
		return reinterpret_cast<void*>(++header);
	}

	inline MemAllocInfo* detachMemAllocInfo(void* ptr)
	{
		MemAllocInfo* header = reinterpret_cast<MemAllocInfo*>(ptr);
		--header;

		return header;
	}

	__declspec(align(MEMORY_ALLOCATION_ALIGNMENT))
	class SmallSizeMemoryPool
	{
	public:
		SmallSizeMemoryPool(DWORD allocSize_);

		MemAllocInfo* pop();
		void push(MemAllocInfo* ptr);

	private:
		SLIST_HEADER freeList;
		const DWORD allocSize;
		volatile long allocCount = 0;
	};

	class MemoryPool
	{
	public:
		static MemoryPool* getInstance();

		void* allocate(unsigned int size);
		void deallocate(void* ptr, long extraInfo);

	private:
		MemoryPool();
		static MemoryPool* pool;

		enum Config
		{
			MAX_POOL_COUNT = 48,
			MAX_CHUNK_SIZE = 32768
		};

		SmallSizeMemoryPool* smallSizeMemoryPoolTable[MAX_CHUNK_SIZE + 1];
	};

	struct PooledAllocatable {};

	template<typename T, typename... Args>
	T* xnew(Args... args)
	{
		static_assert(true == std::is_convertible<T, PooledAllocatable>::value, "only allowed when PooledAllocatable");

		void* alloc = MemoryPool::getInstance()->allocate(sizeof(T));
		new (alloc)T(args...);
		return reinterpret_cast<T*>(alloc);
	}

	template<typename T>
	void xdelete(T* object)
	{
		static_assert(true == std::is_convertible<T, PooledAllocatable>::value, "only allowed when PooledAllocatable");

		object->~T();
		MemoryPool::getInstance()->deallocate(object, sizeof(T));
	}
}