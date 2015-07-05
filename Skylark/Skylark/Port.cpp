#include "stdafx.h"
#include "Port.h"
#include "Exception.h"
#include "Context.h"

skylark::Port::Port()
{
	completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	CRASH_ASSERT(completionPort != NULL);
}

skylark::Port::~Port()
{
	CloseHandle(completionPort);
}

void skylark::Port::job(int timeout)
{
	DWORD transferred = 0;
	LPOVERLAPPED overlapped = nullptr;

	ULONG_PTR completionKey = 0;

	int ret = GetQueuedCompletionStatus(completionPort, &transferred, (PULONG_PTR)&completionKey, &overlapped, timeout);

	Context* context = reinterpret_cast<Context*>(overlapped);

	if (ret == 0 || transferred == 0)
	{
		//check timeout
		if (context == nullptr && GetLastError() == WAIT_TIMEOUT)
			return;
	}
	
	bool completionOk = context->onComplete(transferred, completionKey);

	if (!completionOk)
	{
		context->onFailure();
	}

	delete context;
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

bool skylark::Port::take(Context * context, int key)
{
	return PostQueuedCompletionStatus(completionPort, 0, (ULONG_PTR)key, (LPOVERLAPPED)context) == TRUE;
}

bool skylark::postContext(Port * port, Context * context, int key)
{
	return port->take(context, key);
}
