#pragma once

namespace skylark
{
class Lock;

class LockOrderChecker
{
public:
	LockOrderChecker();

	void push(Lock* lock);
	void pop(Lock* lock);

private:
	static const int MAX_LOCK_DEPTH = 32;

	int mStackTopPos;
	Lock* mLockStack[MAX_LOCK_DEPTH];
};
}