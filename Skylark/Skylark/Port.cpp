#include "stdafx.h"
#include "Port.h"
#include "Exception.h"

skylark::Port::Port()
{
	completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	CRASH_ASSERT(completionPort != NULL);
}

skylark::Port::~Port()
{
	CloseHandle(completionPort);
}

bool skylark::Port::bind(SOCKET socket)
{
	HANDLE handle = CreateIoCompletionPort((HANDLE)socket, completionPort, 0, 0);

	if (handle != completionPort)
	{
		return false;
	}

	return true;
}
