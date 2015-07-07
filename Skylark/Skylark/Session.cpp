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

bool skylark::Session::send(std::int8_t * packet, std::size_t len)
{
	return false;
}

bool skylark::Session::flushSend()
{
	return false;
}
