#pragma once

namespace skylark
{

class Thread
{
public:
	Thread();
	virtual ~Thread() = default;

	virtual void init();
	virtual void run();
	virtual int wait();

	int getId() { return mId; }

private:
	int mId;
	static std::atomic<int> mIdMaker;
	std::thread mThread;
};

extern thread_local Thread* thisThread;

}