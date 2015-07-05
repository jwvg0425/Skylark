#include "stdafx.h"
#include "Port.h"
#include "Socket.h"
#include "Exception.h"


//auto WSASStartup
namespace skylark
{
	struct SocketInitializer
	{
		SocketInitializer()
		{
			WSADATA wsa;
			if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
				CRASH_ASSERT(false);

			//init ex functions
		}
	};

	Socket::Socket(ConnectType type)
	{
		switch (type)
		{
		case ConnectType::TCP:
			socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
			break;
		case ConnectType::UDP:
			socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED);
			break;
		}

		CRASH_ASSERT(socket != INVALID_SOCKET);
	}

	Socket::~Socket()
	{
	}

	void Socket::setOption(SocketOption option)
	{
	}

	bool Socket::completeTo(Port * port)
	{
		bindToPort = true;

		return port->bind(socket);
	}

	bool Socket::bind(const std::string & addr, std::uint16_t port)
	{
		SOCKADDR_IN serveraddr = { 0, };

		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(port);
		serveraddr.sin_addr.s_addr = inet_addr(addr.c_str());

		if (SOCKET_ERROR == ::bind(socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr)))
		{
			return false;
		}

		return true;
	}

	bool Socket::listen()
	{
		return false;
	}
}