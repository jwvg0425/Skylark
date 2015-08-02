#pragma once

namespace skylark
{
	class LockOrderChecker;
	class Session;
	class Timer;
	
	namespace TLS
	{
#if _MSC_VER >= 1900
		extern thread_local LockOrderChecker* lockOrderChecker;
		extern thread_local std::deque<Session*>* sendRequestSessionList;
		extern thread_local int64_t tickCount;
		extern thread_local Timer* timer;
#elif _MSC_VER <= 1800
		extern __declspec(thread) LockOrderChecker* lockOrderChecker;
		extern __declspec(thread) std::deque<Session*>* sendRequestSessionList;
		extern __declspec(thread) int64_t tickCount;
		extern __declspec(thread) Timer* timer;
#endif
	}
}