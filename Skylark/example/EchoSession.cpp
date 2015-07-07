#include "EchoSession.h"

bool EchoSession::onRead()
{
	std::size_t len = recvBuffer.getContiguiousBytes();
	send(recvBuffer.getBuffer(), len);

	recvBuffer.remove(len);

	return preRecv();
}
