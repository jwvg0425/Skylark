#include "stdafx.h"
#include "IOThread.h"
#include "ThreadLocal.h"
#include "LockOrderChecker.h"
#include "CompletionPort.h"
#include "Session.h"
#include "Timer.h"

namespace skylark
{
	void ioThreadFunc(IOThread* thread)
	{
		thread->init();

		while (true)
		{
			thread->ioJob();
			TLS::timer->doTimerJob();
			thread->sendJob();
			thread->customJob();
		}
	}
}

void skylark::IOThread::ioJob()
{
	port->job();
}

void skylark::IOThread::customJob()
{
	if (customFunc != nullptr)
		customFunc();
}

skylark::IOThread::IOThread(int id_, CompletionPort* port_, std::function<void()> customFunc_) :
	id(id_), port(port_), customFunc(customFunc_), t(ioThreadFunc, this)
{

}

skylark::IOThread::IOThread(int id_, CompletionPort * port_)
	:id(id_), port(port_), customFunc(nullptr), t(ioThreadFunc, this)
{
}

skylark::IOThread::~IOThread()
{
}

void skylark::IOThread::sendJob()
{
	while (!TLS::sendRequestSessionList->empty())
	{
		auto& session = TLS::sendRequestSessionList->front();

		if (session->flushSend())
		{
			TLS::sendRequestSessionList->pop_front();
		}
	}
}

void skylark::IOThread::init()
{
	TLS::lockOrderChecker = new LockOrderChecker(id);
	TLS::sendRequestSessionList = new std::deque<Session*>();
	TLS::timer = new Timer();
	TLS::tickCount = GetTickCount64();
}

int skylark::IOThread::wait()
{
	t.join();

	return 0;
}

int skylark::IOThread::getId()
{
	return id;
}
