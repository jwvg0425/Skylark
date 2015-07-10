#pragma once

namespace skylark
{
	class LockOrderChecker;
	class Session;
	
	namespace TLS
	{
		extern thread_local LockOrderChecker* lockOrderChecker;
		extern thread_local std::deque<Session*>* sendRequestSessionList;
	}
}