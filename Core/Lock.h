#pragma once


namespace skylark
{

class Lock
{
public:
	using Order = unsigned char;

	Lock(Order order);
	virtual ~Lock() = default;

	virtual void enterWriteLock() = 0;
	virtual void leaveWriteLock() = 0;

	virtual void enterReadLock() = 0;
	virtual void leaveReadLock() = 0;

	bool operator <(const Lock& rhs);
	bool operator >(const Lock& rhs);

private:
	const Order mOrder;
};

template <typename LType>
class Guard
{
public:
	Guard(LType& lock, bool exclusive)
		: mLock(lock), mExclusive(exclusive)
	{
		if (mExclusive)
			mLock.enterWriteLock();
		else
			mLock.enterReadLock();
	}

	~Guard()
	{
		if (mExclusive)
			mLock.leaveWriteLock();
		else
			mLock.leaveReadLock();
	}

private:
	LType& mLock;
	bool mExclusive;
};

}