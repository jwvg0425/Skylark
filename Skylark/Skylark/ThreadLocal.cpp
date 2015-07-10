#include "stdafx.h"
#include "ThreadLocal.h"
#include "LockOrderChecker.h"
#include "Session.h"

__declspec(thread) skylark::LockOrderChecker* skylark::TLS::lockOrderChecker = nullptr;
__declspec(thread) std::deque<skylark::Session*>* skylark::TLS::sendRequestSessionList = nullptr;