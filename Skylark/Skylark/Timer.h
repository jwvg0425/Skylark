#pragma once
#include "XTL.h"
#include "MPL.h"

namespace skylark
{

class SyncExecutable;
typedef std::function<void()> TimerTask;
typedef std::shared_ptr<SyncExecutable> SyncExecutablePtr;

struct TimerJobElement
{
	TimerJobElement(SyncExecutablePtr owner_, const TimerTask& task_, int64_t execTick)
		: owner(owner_), task(task_), executionTick(execTick)
	{}

	SyncExecutablePtr owner;
	TimerTask	task;
	int64_t		executionTick;
};

struct TimerJobComparator
{
	bool operator()(const TimerJobElement& lhs, const TimerJobElement& rhs)
	{
		return lhs.executionTick > rhs.executionTick;
	}
};


typedef std::priority_queue<TimerJobElement, std::vector<TimerJobElement, STLAllocator<TimerJobElement> >, TimerJobComparator> TimerJobPriorityQueue;

class Timer
{
public:

	Timer();

	void pushTimerJob(SyncExecutablePtr owner, const TimerTask& task, uint32_t after);

	void doTimerJob();

private:

	TimerJobPriorityQueue	timerJobQueue;

};

}