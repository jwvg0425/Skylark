#include "stdafx.h"
#include "Context.h"
#include "Session.h"

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
	if(transferred == 0)
		return false;

	return session->onRead();
}
