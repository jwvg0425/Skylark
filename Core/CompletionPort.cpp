#include "stdafx.h"
#include "CompletionPort.h"
#include "Exception.h"
#include "Context.h"

skylark::CompletionPort::CompletionPort(int timeout)
	:mTimeout(timeout)
{
	mCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	crashAssert(mCompletionPort != NULL);
}

skylark::CompletionPort::~CompletionPort()
{
	CloseHandle(mCompletionPort);
}

void skylark::CompletionPort::job() const
{
	DWORD transferred = 0;
	LPOVERLAPPED overlapped = nullptr;

	ULONG_PTR completionKey = 0;

	int ret = GetQueuedCompletionStatus(mCompletionPort, &transferred, (PULONG_PTR)&completionKey, &overlapped, mTimeout);

	if (ret == 0 || transferred == 0)
	{
		//check timeout
		if (overlapped == nullptr && GetLastError() == WAIT_TIMEOUT)
			return;
	}

	Overlapped* real = reinterpret_cast<Overlapped*>(overlapped);
	Context* context = (overlapped == nullptr) ? nullptr : real->mContext;


	crashAssert(context != nullptr);

	bool completionOk = context->onComplete(transferred, completionKey);

	if (!completionOk)
	{
		context->onFailure();
	}

	delete real;
}

bool skylark::CompletionPort::bind(SOCKET socket)
{
	HANDLE handle = CreateIoCompletionPort((HANDLE)socket, mCompletionPort, 0, 0);

	if (handle != mCompletionPort)
	{
		return false;
	}

	return true;
}

bool skylark::CompletionPort::take(Context* context, int key)
{
	Overlapped* overlapped = new Overlapped(context);

	return PostQueuedCompletionStatus(mCompletionPort, 0, (ULONG_PTR)key, (LPOVERLAPPED)overlapped) == TRUE;
}

bool skylark::postContext(CompletionPort* port, Context* context, int key)
{
	return port->take(context, key);
}