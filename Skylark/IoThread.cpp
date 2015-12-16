#include "stdafx.h"
#include "IOThread.h"
#include "LockOrderChecker.h"
#include "CompletionPort.h"
#include "Session.h"


skylark::IOThread::IOThread(CompletionPort * port)
	: mPort(port)
{
}

void skylark::IOThread::init()
{
}

void skylark::IOThread::run()
{
	while (true)
	{
		mPort->job();
	}
}
