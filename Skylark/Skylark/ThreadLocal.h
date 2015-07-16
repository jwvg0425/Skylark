#pragma once

namespace skylark
{
	class LockOrderChecker;
	class Session;
	
	namespace TLS
	{
#if _MSC_VER >= 1900
		extern thread_local LockOrderChecker* lockOrderChecker;
		extern thread_local std::deque<Session*>* sendRequestSessionList;
#elif _MSC_VER <= 1800
		extern __declspec(thread) LockOrderChecker* lockOrderChecker;
		extern __declspec(thread) std::deque<Session*>* sendRequestSessionList;
#endif
	}
}