#include "stdafx.h"
#include "Port.h"

bool skylark::Port::bind(SOCKET socket)
{
	HANDLE handle = CreateIoCompletionPort((HANDLE)socket, completionPort, 0, 0);

	if (handle != completionPort)
	{
		return false;
	}

	return true;
}
