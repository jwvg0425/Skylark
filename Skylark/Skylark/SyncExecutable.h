#pragma once

#include "Lock.h"
#include "Timer.h"

namespace skylark
{

class SyncExecutable : public std::enable_shared_from_this<SyncExecutable>
{
public:
	SyncExecutable() : lock(Lock::Order::BUSINESS_CLASS) {}
	virtual ~SyncExecutable() {}

	template<class R, class T, class... Args>
	R doSync(R(T::*memfunc)(Args...), Args... args)
	{
		static_assert(true == std::is_convertible<T, SyncExecutable>::value, "T should be derived from SyncExecutable");

		Guard guard(lock, true);
		return (static_cast<T*>(this)->*memfunc)(args...);
	}

	void enterLock() { lock.enterWriteLock(); }
	void leaveLock() { lock.leaveWriteLock(); }

	template<class T>
	std::shared_ptr<T> getSharedFromThis()
	{
		static_assert(true == std::is_Convertible<T, SyncExecutable>::value, "T should be derived from SyncExecutable");

		return std::static_pointer_cast<T>(shared_from_this());
	}

private:
	Lock lock;
};

template <class T, class F, class... Args>
void doSyncAfter(uint32_t after, T instance, F memfunc, Args&&... args)
{
	static_assert(true == is_shared_ptr<T>::value, "T should be shared_ptr");
	static_assert(true == std::is_convertible<T, std::shared_ptr<SyncExecutable>>::value, "T should be shared_ptr SyncExecutable");

	auto bind = std::bind(memfunc, instance, std::forward<Args>(args)...);
	TLS::timer->pushTimerJob(std::static_pointer_cast<SyncExecutable>(instance), bind, after);
}

}