#include "stdafx.h"
#include "Context.h"
#include "Session.h"
#include "Socket.h"

bool skylark::AcceptContext::onComplete(int transferred, int key)
{
	return session->onAccept(listen);
}

bool skylark::PreRecvContext::onComplete(int transferred, int key)
{
	return session->recv();
}

bool skylark::RecvContext::onComplete(int transferred, int key)
{
	if (transferred == 0)
	{
		DisconnectContext* context = new DisconnectContext(session, 0/* temp value*/);
		session->disconnect(context);
	}

	return session->recvCompletion(transferred);
}

bool skylark::SendContext::onComplete(int transferred, int key)
{
	if (transferred == 0)
	{
		DisconnectContext* context = new DisconnectContext(session, 0/* temp value*/);
		session->disconnect(context);
	}

	return session->sendCompletion(transferred);
}


bool skylark::DisconnectContext::onComplete(int transferred, int key)
{
	return false;
}

bool skylark::RecvContext::onFailure()
{
	DisconnectContext* context = new DisconnectContext(session, 0/* temp value*/);

	return session->disconnect(context);
}

bool skylark::SendContext::onFailure()
{
	DisconnectContext* context = new DisconnectContext(session, 0/* temp value*/);

	return session->disconnect(context);
}