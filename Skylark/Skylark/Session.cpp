#include "stdafx.h"
#include "Session.h"
#include "Socket.h"
#include "Context.h"
#include "ThreadLocal.h"
#include "Exception.h"

skylark::Session::Session(CompletionPort * port_, std::size_t sendBufSize, std::size_t recvBufSize)
	:socket(new Socket(ConnectType::TCP)), port(port_), sendBuffer(sendBufSize), recvBuffer(recvBufSize), sendLock(Lock::Order::LUGGAGE_CLASS), connected(false)
{
}

skylark::Session::~Session()
{
	delete socket;
}

bool skylark::Session::onDisconnect(int reason)
{
	return true;
}

bool skylark::Session::preRecv()
{
	if (!isConnected())
		return false;

	PreRecvContext* context = new PreRecvContext(this);

	context->wsabuf.len = 0;
	context->wsabuf.buf = nullptr;

	return socket->recv(context, context->wsabuf);
}

bool skylark::Session::recv()
{
	if (!isConnected())
		return false;

	if (0 == recvBuffer.getFreeSpaceSize())
		return false;

	RecvContext* recvContext = new RecvContext(this);

	DWORD recvbytes = 0;
	DWORD flags = 0;
	recvContext->wsabuf.len = (ULONG)recvBuffer.getFreeSpaceSize();
	recvContext->wsabuf.buf = (char*)recvBuffer.getBuffer();

	return socket->recv(recvContext, recvContext->wsabuf);
}

bool skylark::Session::acceptCompletion(Socket* listen)
{
	if (true == connected.exchange(true))
	{
		CRASH_ASSERT(false);
		return false;
	}

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

	return onAccept();
}

bool skylark::Session::onAccept()
{
	return true;
}

bool skylark::Session::onRead()
{
	return true;
}

bool skylark::Session::onSend()
{
	return true;
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

void skylark::Session::addRef()
{
	refCount++;
}

void skylark::Session::releaseRef()
{
	refCount--;

	CRASH_ASSERT(refCount >= 0);

	if (refCount == 0)
	{
		onRelease();
	}
}

void skylark::Session::onRelease()
{

}

bool skylark::Session::send(char * packet, std::size_t len)
{
	if (!isConnected())
		return false;

	Guard guard(sendLock, true);

	if (sendBuffer.getFreeSpaceSize() < len)
		return false;

	TLS::sendRequestSessionList->push_back(this);

	sendBuffer.write(packet, len);

	return true;
}

bool skylark::Session::flushSend()
{
	if (!isConnected())
	{
		DisconnectContext* context = new DisconnectContext(this, 0/* temp value*/);
		disconnect(context);
		return false;
	}

	Guard guard(sendLock, true);

	if (0 == sendBuffer.getContiguiousBytes())
	{
		if (0 == sendPendingCount)
			return true;

		return false;
	}

	if(sendPendingCount > 0)
		return false;

	SendContext* context = new SendContext(this);

	DWORD sendbytes = 0;
	DWORD flags = 0;

	context->wsabuf.len = (ULONG)sendBuffer.getContiguiousBytes();
	context->wsabuf.buf = (char*)sendBuffer.getBufferStart();

	if (!socket->send(context, context->wsabuf))
	{
		DisconnectContext* context = new DisconnectContext(this, 0/* temp value*/);
		disconnect(context);
		return true;
	}

	sendPendingCount++;

	return sendPendingCount == 1;
}

bool skylark::Session::isConnected()
{
	return connected;
}