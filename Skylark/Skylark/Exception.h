#pragma once

namespace skylark
{
	inline void CRASH_ASSERT(bool isOk)
	{
		if (isOk)
			return;

		int* crashVal = nullptr;
		*crashVal = 0xDEADBEEF;
	}
}