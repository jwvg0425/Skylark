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
			thread->job();
		}
	}
}

void skylark::IOThread::ioJob()
{
	port->job();
}

skylark::IOThread::IOThread(int id_, CompletionPort * port_)
	:id(id_), port(port_), t(ioThreadFunc, this)
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
		TLS::sendRequestSessionList->pop_front();

		if (!session->flushSend())
		{
			TLS::sendRequestFailedList->push_front(session);
		}

		std::deque<Session*>* tmp;

		tmp = TLS::sendRequestSessionList;
		TLS::sendRequestSessionList = TLS::sendRequestFailedList;
		TLS::sendRequestFailedList = tmp;
	}
}

void skylark::IOThread::init()
{
	TLS::lockOrderChecker = new LockOrderChecker(id);
	TLS::sendRequestSessionList = new std::deque<Session*>();
	TLS::sendRequestFailedList = new std::deque<Session*>();
	TLS::timer = new Timer();
	TLS::tickCount = GetTickCount64();
}

void skylark::IOThread::job()
{
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
