#include "stdafx.h"
#include "Session.h"
#include "Socket.h"

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
	return false;
}

bool skylark::Session::recv()
{
	return false;
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
	}

	return true;
}

bool skylark::Session::send(std::int8_t * packet, std::size_t len)
{
	return false;
}

bool skylark::Session::flushSend()
{
	return false;
}
