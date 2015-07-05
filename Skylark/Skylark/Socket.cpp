#include "stdafx.h"
#include "Port.h"
#include "Socket.h"
#include "Exception.h"


//auto WSASStartup
namespace skylark
{
	LPFN_DISCONNECTEX disconnectEx_ = nullptr;
	LPFN_ACCEPTEX acceptEx_ = nullptr;
	LPFN_CONNECTEX connectEx_ = nullptr;

	struct SocketInitializer
	{
		SocketInitializer()
		{
			WSADATA wsa;
			if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
				CRASH_ASSERT(false);

			//init ex functions
			SOCKET initEx = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
			CRASH_ASSERT(initEx != INVALID_SOCKET);

			GUID guidDisconnectEx = WSAID_DISCONNECTEX;
			DWORD bytes = 0;

			if (SOCKET_ERROR == WSAIoctl(initEx, SIO_GET_EXTENSION_FUNCTION_POINTER,
				&guidDisconnectEx, sizeof(GUID), &disconnectEx_, sizeof(LPFN_DISCONNECTEX), &bytes, NULL, NULL))
				CRASH_ASSERT(false);

			GUID guidAcceptEx = WSAID_ACCEPTEX;
			if (SOCKET_ERROR == WSAIoctl(initEx, SIO_GET_EXTENSION_FUNCTION_POINTER,
				&guidAcceptEx, sizeof(GUID), &acceptEx_, sizeof(LPFN_ACCEPTEX), &bytes, NULL, NULL))
				CRASH_ASSERT(false);

			GUID guidConnectEx = WSAID_CONNECTEX;
			if (SOCKET_ERROR == WSAIoctl(initEx, SIO_GET_EXTENSION_FUNCTION_POINTER,
				&guidConnectEx, sizeof(GUID), &connectEx_, sizeof(LPFN_CONNECTEX), &bytes, NULL, NULL))
				CRASH_ASSERT(false);
		}

		~SocketInitializer()
		{
			WSACleanup();
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
		closesocket(socket);
	}

	bool Socket::reuseAddr(bool reuse)
	{
		int opt = reuse ? 1 : 0;

		return SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int));
	}

	bool Socket::setLinger(bool lingerOn, int lingerTime)
	{
		LINGER lingerOption;
		lingerOption.l_onoff = lingerOn ? 1 : 0;
		lingerOption.l_linger = lingerTime;

		return SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_LINGER, (const char*)&lingerOption, sizeof(LINGER));
	}

	bool Socket::updateAcceptContext(Socket * s)
	{
		return SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&s->socket, sizeof(SOCKET));
	}

	bool Socket::setNodelay(bool nodelay)
	{
		int opt = nodelay ? 1 : 0;
		return SOCKET_ERROR == setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int));
	}

	bool Socket::setReceiveBufferSize(int size)
	{
		return SOCKET_ERROR == setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(int));
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