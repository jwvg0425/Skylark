#include "stdafx.h"
#include "Session.h"
#include "Socket.h"
#include "Context.h"

skylark::Session::Session(Port * port_, std::size_t sendBufSize, std::size_t recvBufSize)
	:socket(new Socket(ConnectType::TCP)), port(port_), sendBuffer(sendBufSize), recvBuffer(recvBufSize), sendLock(Lock::Order::LUGGAGE_CLASS)
{
}

skylark::Session::~Session()
{
	delete socket;
}

void skylark::Session::disconnect(int reason)
{
}

bool skylark::Session::preRecv()
{
	PreRecvContext* context = new PreRecvContext(this);

	context->wsabuf.len = 0;
	context->wsabuf.buf = nullptr;

	return socket->recv(context, context->wsabuf);
}

bool skylark::Session::recv()
{
	if (0 == recvBuffer.getFreeSpaceSize())
		return false;

	RecvContext* recvContext = new RecvContext(this);

	DWORD recvbytes = 0;
	DWORD flags = 0;
	recvContext->wsabuf.len = (ULONG)recvBuffer.getFreeSpaceSize();
	recvContext->wsabuf.buf = (char*)recvBuffer.getBuffer();

	return socket->recv(recvContext, recvContext->wsabuf);
}

bool skylark::Session::onAccept(Socket* listen)
{
	bool resultOk = true;
	do
	{
		if (!socket->updateAcceptContext(listen))
		{
			resultOk = false;
			break;
		}

		if (!socket->setNodelay(true))
		{
			resultOk = false;
			break;
		}

		if (!socket->setReceiveBufferSize(0))
		{
			resultOk = false;
			break;
		}

		if (!socket->completeTo(port))
		{
			resultOk = false;
			break;
		}
	} while (false);

	if (!resultOk)
	{
		return false;
	}

	if (false == preRecv())
	{
		printf("pre recv failed.\n");
	}

	return true;
}

bool skylark::Session::onRead()
{
	return false;
}

bool skylark::Session::send(std::int8_t * packet, std::size_t len)
{
	return false;
}

bool skylark::Session::flushSend()
{
	return false;
}
