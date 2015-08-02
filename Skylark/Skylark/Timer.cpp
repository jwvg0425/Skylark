#include "stdafx.h"
#include "Timer.h"
#include "ThreadLocal.h"
#include "SyncExecutable.h"

skylark::Timer::Timer()
{
	TLS::tickCount = GetTickCount64();
}


void skylark::Timer::pushTimerJob(SyncExecutablePtr owner, const TimerTask& task, uint32_t after)
{
	int64_t dueTimeTick = after + TLS::tickCount;

	timerJobQueue.push(TimerJobElement(owner, task, dueTimeTick));
}


void skylark::Timer::doTimerJob()
{
	/// thread tick update
	TLS::tickCount = GetTickCount64();

	while (!timerJobQueue.empty())
	{
		const TimerJobElement timerJobElem = timerJobQueue.top();

		if (TLS::tickCount < timerJobElem.executionTick)
			break;

		timerJobElem.owner->enterLock();
		timerJobElem.task();
		timerJobElem.owner->leaveLock();

		timerJobQueue.pop();
	}


}