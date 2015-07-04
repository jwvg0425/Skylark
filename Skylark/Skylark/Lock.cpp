#include "stdafx.h"
#include "Lock.h"
#include "ThreadLocal.h"
#include "LockOrderChecker.h"

skylark::Lock::Lock(Order order_) : lockFlag(0), order(order_)
{

}

skylark::Lock::~Lock()
{

}

void skylark::Lock::enterWriteLock()
{
	if (order != Order::DONT_CARE)
		TLS::lockOrderChecker->push(this);

	while (true)
	{
		while (lockFlag & WRITE_MASK)
			YieldProcessor();

		if ((InterlockedAdd(&lockFlag, WRITE_FLAG) & WRITE_MASK) == WRITE_FLAG)
		{
			while (lockFlag & READ_MASK)
				YieldProcessor();

			return;
		}

		InterlockedAdd(&lockFlag, -WRITE_FLAG);
	}
}

void skylark::Lock::leaveWriteLock()
{
	InterlockedAdd(&lockFlag, -WRITE_FLAG);

	if (order != Order::DONT_CARE)
		TLS::lockOrderChecker->pop(this);
}

void skylark::Lock::enterReadLock()
{
	if (order != Order::DONT_CARE)
		TLS::lockOrderChecker->push(this);

	while (true)
	{
		while (lockFlag & WRITE_MASK)
			YieldProcessor();

		if ((InterlockedIncrement(&lockFlag) & WRITE_MASK) == 0)
		{
			return;
		}

		InterlockedDecrement(&lockFlag);
	}
}

void skylark::Lock::leaveReadLock()
{
	InterlockedDecrement(&lockFlag);

	if (order != Order::DONT_CARE)
		TLS::lockOrderChecker->pop(this);
}

bool skylark::Lock::operator<(const Lock & rhs)
{
	return order > rhs.order;
}

bool skylark::Lock::operator>(const Lock & rhs)
{
	return order < rhs.order;
}

