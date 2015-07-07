#pragma once
#include "Skylark.h"

class EchoSession : public skylark::Session
{
public:
	EchoSession(skylark::Port* port, std::size_t sendBufSize, std::size_t recvBufSize)
		:Session(port, sendBufSize, recvBufSize) {}
	~EchoSession() override = default;

	bool onRead() override;

private:
};