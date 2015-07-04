#include "stdafx.h"
#include "Exception.h"
#include "LockOrderChecker.h"

skylark::LockOrderChecker::LockOrderChecker(int tid)
	:workerThreadId(tid), stackTopPos(0)
{
	memset(lockStack, 0, sizeof(lockStack));
}

void skylark::LockOrderChecker::push(Lock * lock)
{
	//depth check
	CRASH_ASSERT(stackTopPos < MAX_LOCK_DEPTH);

	if (stackTopPos > 0)
	{
		//order check
		CRASH_ASSERT(lockStack[stackTopPos-1] < lock);
	}

	lockStack[stackTopPos++] = lock;
}

void skylark::LockOrderChecker::pop(Lock * lock)
{
	//depth check
	CRASH_ASSERT(stackTopPos > 0);

	//order check
	CRASH_ASSERT(lockStack[stackTopPos-1] == lock);

	lockStack[--stackTopPos] = nullptr;
}
