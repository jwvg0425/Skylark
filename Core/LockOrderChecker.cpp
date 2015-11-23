#include "stdafx.h"
#include "Exception.h"
#include "LockOrderChecker.h"
#include "Lock.h"

skylark::LockOrderChecker::LockOrderChecker()
	:mStackTopPos(0)
{
	memset(mLockStack, 0, sizeof(mLockStack));
}

void skylark::LockOrderChecker::push(Lock* lock)
{
	//depth check
	crashAssert(mStackTopPos < MAX_LOCK_DEPTH);

	if (mStackTopPos > 0)
	{
		//order check
		crashAssert((*mLockStack[mStackTopPos - 1]) < (*lock));
	}

	mLockStack[mStackTopPos++] = lock;
}

void skylark::LockOrderChecker::pop(Lock* lock)
{
	//depth check
	crashAssert(mStackTopPos > 0);

	//order check
	crashAssert(mLockStack[mStackTopPos - 1] == lock);

	mLockStack[--mStackTopPos] = nullptr;
}
