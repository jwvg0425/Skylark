#pragma once

namespace skylark
{

class LockOrderChecker;

class Thread
{
public:
	Thread();
	virtual ~Thread() = default;

	virtual void init();
	virtual void run();
	virtual int wait();

	int getId() { return mId; }

	LockOrderChecker* getLockOrderChecker() { return mLockOrderChecker; }

private:
	int mId;
	static std::atomic<int> mIdMaker;
	std::thread mThread;

	LockOrderChecker* mLockOrderChecker;
};

extern thread_local Thread* thisThread;

}