#include "EchoApplication.h"

EchoApplication::EchoApplication(int threadNum_)
	: threadNum(threadNum_)
{
}

EchoApplication::~EchoApplication()
{
}

int EchoApplication::run()
{
	for (auto thread : threads)
	{
		thread->run();
	}

	return 0;
}

bool EchoApplication::init()
{
	mainPort = new skylark::Port(10);
	listen = new skylark::Socket(skylark::ConnectType::TCP);
	if (!listen->completeTo(mainPort))
		return false;

	if (!listen->reuseAddr(true))
		return false;
	
	if (!listen->bind("127.0.0.1", 41010))
		return false;

	if (!listen->listen())
		return false;

	for (int i = 0; i < 4; i++)
	{
		auto thread = new skylark::IOThread(i, mainPort, [] {});

		threads.push_back(thread);
	}

	return true;
}
