#include "stdafx.h"
#include "ThreadLocal.h"
#include "LockOrderChecker.h"
#include "Session.h"
#include "Timer.h"

#if _MSC_VER >= 1900
thread_local skylark::LockOrderChecker* skylark::TLS::lockOrderChecker = nullptr;
thread_local std::deque<skylark::Session*>* skylark::TLS::sendRequestSessionList = nullptr;
thread_local std::deque<skylark::Session*>* skylark::TLS::sendRequestFailedList = nullptr;
thread_local int64_t skylark::TLS::tickCount = 0;
thread_local int skylark::TLS::threadId = -1;
thread_local skylark::Timer* skylark::TLS::timer = nullptr;

#elif MSC_VER <= 1800
__declspec(thread) skylark::LockOrderChecker* skylark::TLS::lockOrderChecker = nullptr;
__declspec(thread) std::deque<skylark::Session*>* skylark::TLS::sendRequestSessionList = nullptr;
__declspec(thread) std::deque<skylark::Session*>* skylark::TLS::sendRequestFailedList = nullptr;
__declspec(thread) int64_t skylark::TLS::tickCount = 0;
__declspec(thread) int skylark::TLS::threadId = -1;
__declspec(thread) Timer* skylark::TLS::timer = nullptr;
#endif