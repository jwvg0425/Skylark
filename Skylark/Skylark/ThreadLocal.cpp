#include "stdafx.h"
#include "ThreadLocal.h"
#include "LockOrderChecker.h"

thread_local skylark::LockOrderChecker* skylark::TLS::lockOrderChecker = nullptr;