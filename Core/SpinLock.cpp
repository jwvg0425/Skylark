#include "stdafx.h"
#include "SpinLock.h"
#include "Thread.h"
#include "LockOrderChecker.h"

void skylark::SpinLock::enterWriteLock()
{
	thisThread->getLockOrderChecker()->push(this);

	while (true)
	{
		while (mLockFlag & WRITE_MASK)
			YieldProcessor();

		if (((mLockFlag += WRITE_FLAG) & WRITE_MASK) == WRITE_FLAG)
		{
			while (mLockFlag & READ_MASK)
				YieldProcessor();

			return;
		}

		mLockFlag -= WRITE_FLAG;
	}
}

void skylark::SpinLock::leaveWriteLock()
{
	mLockFlag -= WRITE_FLAG;

	thisThread->getLockOrderChecker()->pop(this);
}

void skylark::SpinLock::enterReadLock()
{
	thisThread->getLockOrderChecker()->push(this);

	while (true)
	{
		while (mLockFlag & WRITE_MASK)
			YieldProcessor();

		if ((++mLockFlag & WRITE_MASK) == 0)
		{
			return;
		}

		--mLockFlag;
	}
}

void skylark::SpinLock::leaveReadLock()
{
	--mLockFlag;

	thisThread->getLockOrderChecker()->pop(this);
}
