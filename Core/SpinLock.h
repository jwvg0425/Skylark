#pragma once
#include "Lock.h"


namespace skylark
{

class SpinLock : public Lock
{
public:
	SpinLock(Order order) : Lock(order) {}
	~SpinLock() override = default;

	void enterWriteLock() override;
	void leaveWriteLock() override;

	void enterReadLock() override;
	void leaveReadLock() override;

private:
	enum mLockFlag
	{
		WRITE_MASK = 0x7FF00000,
		WRITE_FLAG = 0x00100000,
		READ_MASK = 0x000FFFFF
	};

	std::atomic<long> mLockFlag;
};

}