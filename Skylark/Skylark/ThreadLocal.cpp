#include "stdafx.h"
#include "ThreadLocal.h"
#include "LockOrderChecker.h"

thread_local int skylark::TLS::threadType = -1;
thread_local int skylark::TLS::workerThreadId = -1;
thread_local skylark::LockOrderChecker* skylark::TLS::lockOrderChecker = nullptr;