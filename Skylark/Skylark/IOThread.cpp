#include "stdafx.h"
#include "IOThread.h"
#include "ThreadLocal.h"
#include "LockOrderChecker.h"
#include "Port.h"

skylark::IOThread::IOThread(int id_, Port* port_, std::function<void()> customFunc) :
	id(id_), port(port_), t([this, customFunc]()
	{
		init();

		while (true)
		{
			port->job();
			sendJob();
			customFunc();
		}
	})
{

}

skylark::IOThread::IOThread(int id_, Port * port_)
	:id(id_), port(port_), t([this]()
	{
		init();

		while (true)
		{
			port->job();
			sendJob();
		}
	})
{
}

skylark::IOThread::~IOThread()
{
}

void skylark::IOThread::sendJob()
{
}

void skylark::IOThread::init()
{
	TLS::lockOrderChecker = new LockOrderChecker(id);
}

int skylark::IOThread::run()
{
	t.join();

	return 0;
}
