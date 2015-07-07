#include "stdafx.h"
#include "Session.h"
#include "Socket.h"
#include "Context.h"
#include "ThreadLocal.h"

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

bool skylark::Session::onSend()
{
	return false;
}

bool skylark::Session::sendCompletion(DWORD transferred)
{
	Guard guard(sendLock, true);

	sendBuffer.remove(transferred);
	sendPendingCount--;

	return onSend();
}

bool skylark::Session::recvCompletion(DWORD transferred)
{
	recvBuffer.commit(transferred);

	return onRead();
}

bool skylark::Session::send(std::int8_t * packet, std::size_t len)
{
	Guard guard(sendLock, true);

	if (sendBuffer.getFreeSpaceSize() < len)
		return false;

	TLS::sendRequestSessionList->push_back(this);

	auto destData = sendBuffer.getBuffer();

	memcpy(destData, packet, len);

	sendBuffer.commit(len);

	return true;
}

bool skylark::Session::flushSend()
{
	if (0 == sendBuffer.getContiguiousBytes())
	{
		if (0 == sendPendingCount)
			return true;

		return false;
	}

	if(sendPendingCount > 0)
		return false;

	Guard guard(sendLock, true);

	SendContext* context = new SendContext(this);

	DWORD sendbytes = 0;
	DWORD flags = 0;

	context->wsabuf.len = (ULONG)sendBuffer.getContiguiousBytes();
	context->wsabuf.buf = (char*)sendBuffer.getBufferStart();

	if (!socket->send(context, context->wsabuf))
	{
		//disconnect
		return true;
	}

	sendPendingCount++;

	return sendPendingCount == 1;
}
