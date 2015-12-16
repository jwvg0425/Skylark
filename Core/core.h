#pragma once
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#include <atomic>
#include <thread>

#include <Windows.h>
#include <cstdlib>

#include <WinSock2.h>
#include <MSWSock.h>

#include <cstdint>
#include <cstdio>

#include <type_traits>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "DbgHelp.lib")

#include "Exception.h"
#include "Socket.h"
#include "CompletionPort.h"
#include "Thread.h"
#include "CircularBuffer.h"
#include "Context.h"
#include "Lock.h"
#include "LockOrderChecker.h"
#include "ObjectPool.h"