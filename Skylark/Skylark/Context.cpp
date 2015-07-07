#include "stdafx.h"
#include "Context.h"
#include "Session.h"

bool skylark::AcceptContext::onComplete(int transferred, int key)
{
	return session->onAccept(listen);
}
