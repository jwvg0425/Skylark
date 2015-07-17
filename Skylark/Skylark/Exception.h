#pragma once

namespace skylark
{
	LONG WINAPI exceptionFilter(EXCEPTION_POINTERS* exceptionInfo);

	inline void CRASH_ASSERT(bool isOk)
	{
		if (isOk)
			return;

		int* crashVal = nullptr;
		*crashVal = 0xDEADBEEF;
	}
}