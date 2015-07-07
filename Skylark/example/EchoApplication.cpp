#include "EchoApplication.h"
#include "EchoSession.h"

EchoApplication::EchoApplication(int threadNum_)
	: threadNum(threadNum_)
{
}

EchoApplication::~EchoApplication()
{
}

int EchoApplication::run()
{
	//100 clients accept
	for (int i = 0; i < 100; i++)
	{
		EchoSession* session = new EchoSession(mainPort, 4096, 4096);
		skylark::AcceptContext* context = new skylark::AcceptContext(session, listen);

		session->accept(listen, context);
	}

	printf("start server...\n");

	for (auto thread : threads)
	{
		thread->run();
	}

	printf("end server...\n");

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
		auto thread = new skylark::IOThread(i, mainPort);

		threads.push_back(thread);
	}

	return true;
}
