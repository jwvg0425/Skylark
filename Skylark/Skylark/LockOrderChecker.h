#pragma once

namespace skylark
{
	class Lock;

	class LockOrderChecker
	{
	public:
		LockOrderChecker(int tid);

		void push(Lock* lock);
		void pop(Lock* lock);

	private:
		static const int MAX_LOCK_DEPTH = 32;

		int workerThreadId;
		int stackTopPos;

		Lock* lockStack[MAX_LOCK_DEPTH];
	};
}