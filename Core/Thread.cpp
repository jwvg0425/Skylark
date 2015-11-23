#include "stdafx.h"
#include "Thread.h"
#include "LockOrderChecker.h"

std::atomic<int> skylark::Thread::mIdMaker = 0;
thread_local skylark::Thread* thisThread = nullptr;

void threadFunc(skylark::Thread* thread)
{
	thisThread = thread;

	thread->init();

	thread->run();
}

skylark::Thread::Thread()
	: mId(mIdMaker++), mThread(threadFunc, this), mLockOrderChecker(new LockOrderChecker())
{
}

void skylark::Thread::init()
{
}

void skylark::Thread::run()
{
}

int skylark::Thread::wait()
{
	mThread.join();

	return 0;
}
