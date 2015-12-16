#include "stdafx.h"
#include "SessionContext.h"
#include "Session.h"

bool skylark::AcceptContext::onComplete(int transferred, int key)
{
	return mSession->acceptCompletion(mListen);
}

bool skylark::PreRecvContext::onComplete(int transferred, int key)
{
	return mSession->recv();
}


bool skylark::RecvContext::onComplete(int transferred, int key)
{
	if (transferred == 0)
	{
		DisconnectContext* context = new DisconnectContext(mSession, 0/* temp value*/);
		mSession->disconnect(context);
	}

	return mSession->recvCompletion(transferred);
}

bool skylark::SendContext::onComplete(int transferred, int key)
{
	if (transferred == 0)
	{
		DisconnectContext* context = new DisconnectContext(mSession, 0/* temp value*/);
		mSession->disconnect(context);
	}

	return mSession->sendCompletion(transferred);
}


bool skylark::DisconnectContext::onComplete(int transferred, int key)
{
	return mSession->disconnectCompletion(mReason);
}

void skylark::RecvContext::onFailure()
{
	DisconnectContext* context = new DisconnectContext(mSession, 0/* temp value*/);

	mSession->disconnect(context);
}

void skylark::SendContext::onFailure()
{
	DisconnectContext* context = new DisconnectContext(mSession, 0/* temp value*/);

	mSession->disconnect(context);
}

skylark::UdpContext::UdpContext()
{
	memset(&mAddress, 0, sizeof(SOCKADDR_IN));
}

skylark::UdpContext::UdpContext(const std::string& addr, std::uint16_t port)
{
	mAddress.sin_family = AF_INET;
	mAddress.sin_port = htons(port);
	mAddress.sin_addr.s_addr = inet_addr(addr.c_str());
}

skylark::UdpContext::UdpContext(SOCKADDR_IN address)
	:mAddress(address)
{

}

bool skylark::FunctionContext::onComplete(int transferred, int key)
{
	return mFunc();
}

void skylark::FunctionContext::onFailure()
{
	if (mFailFunc == nullptr)
		return;

	mFailFunc();
}