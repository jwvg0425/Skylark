#include "stdafx.h"
#include "ThreadLocal.h"
#include "LockOrderChecker.h"
#include "Session.h"

thread_local skylark::LockOrderChecker* skylark::TLS::lockOrderChecker = nullptr;
thread_local std::deque<skylark::Session*>* skylark::TLS::sendRequestSessionList = nullptr;