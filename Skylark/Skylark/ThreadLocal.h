#pragma once

namespace skylark
{
	class LockOrderChecker;
	class Session;
	
	namespace TLS
	{
		extern __declspec(thread) LockOrderChecker* lockOrderChecker;
		extern __declspec(thread) std::deque<Session*>* sendRequestSessionList;
	}
}