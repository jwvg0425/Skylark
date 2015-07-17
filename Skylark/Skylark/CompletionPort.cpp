#include "stdafx.h"
#include "CompletionPort.h"
#include "Exception.h"
#include "Context.h"

skylark::CompletionPort::CompletionPort(int timeout_)
	:timeout(timeout_)
{
	completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	CRASH_ASSERT(completionPort != NULL);
}

skylark::CompletionPort::~CompletionPort()
{
	CloseHandle(completionPort);
}

void skylark::CompletionPort::job() const
{
	DWORD transferred = 0;
	LPOVERLAPPED overlapped = nullptr;

	ULONG_PTR completionKey = 0;

	int ret = GetQueuedCompletionStatus(completionPort, &transferred, (PULONG_PTR)&completionKey, &overlapped, timeout);

	if (ret == 0 || transferred == 0)
	{
		//check timeout
		if (overlapped == nullptr && GetLastError() == WAIT_TIMEOUT)
			return;
	}

	Overlapped* real = reinterpret_cast<Overlapped*>(overlapped);
	Context* context = (overlapped == nullptr)? nullptr : real->context;


	CRASH_ASSERT(context != nullptr);

	bool completionOk = context->onComplete(transferred, completionKey);

	if (!completionOk)
	{
		context->onFailure();
	}

	delete overlapped;
}

bool skylark::CompletionPort::bind(SOCKET socket)
{
	HANDLE handle = CreateIoCompletionPort((HANDLE)socket, completionPort, 0, 0);

	if (handle != completionPort)
	{
		return false;
	}

	return true;
}

bool skylark::CompletionPort::take(Context * context, int key)
{
	Overlapped* overlapped = new Overlapped(context);

	return PostQueuedCompletionStatus(completionPort, 0, (ULONG_PTR)key, (LPOVERLAPPED)overlapped) == TRUE;
}

bool skylark::postContext(CompletionPort * port, Context * context, int key)
{
	return port->take(context, key);
}
