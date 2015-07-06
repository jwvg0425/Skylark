// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#include <process.h>
#include <assert.h>
#include <limits.h>

#include <WinSock2.h>
#include <MSWSock.h>

#include <cstdint>
#include <memory>
#include <deque>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <type_traits>
#include <iostream>
#include <sstream>
#include <functional>
#include <thread>

#include <ppl.h>
#include <concurrent_queue.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
