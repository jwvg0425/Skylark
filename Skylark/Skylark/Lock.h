#pragma once

namespace skylark
{
	class LockOrderChecker;

	class Lock
	{
	public:

		enum class Order
		{
			DONT_CARE = 0,
			FIRST_CLASS,
			BUSINESS_CLASS,
			ECONOMY_CLASS,
			LUGGAGE_CLASS
		};

		Lock(Order order_);
		~Lock();

		void enterWriteLock();
		void leaveWriteLock();

		void enterReadLock();
		void leaveReadLock();

		long getLockFlag() const { return lockFlag; }

		//compare by order
		bool operator <(const Lock& rhs);
		bool operator >(const Lock& rhs);

	private:

		enum LockFlag
		{
			WRITE_MASK = 0x7FF00000,
			WRITE_FLAG = 0x00100000,
			READ_MASK  = 0x000FFFFF
		};

		volatile long lockFlag;
		const Order order;
	};

	class Guard
	{
	public:
		Guard(Lock& lock_, bool exclusive_)
			: lock(lock_), exclusive(exclusive_)
		{
			if (exclusive)
				lock.enterWriteLock();
			else
				lock.enterReadLock();
		}

		~Guard()
		{
			if (exclusive)
				lock.leaveWriteLock();
			else
				lock.leaveReadLock();
		}

	private:
		Lock lock;
		bool exclusive;
	};

	template <typename Target>
	class TypeLock
	{
	public:
		struct Guard
		{
			Guard()
			{
				Target::lock.enterWriteLock();
			}

			~Guard()
			{
				Target::lock.leaveWriteLock();
			}
		};

	private:
		static Lock lock;
	};

	template<typename Target>
	Lock TypeLock<Target>::lock;
}