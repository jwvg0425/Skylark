#include "stdafx.h"
#include "CompletionPort.h"
#include "Socket.h"
#include "Exception.h"
#include "Context.h"


//auto WSASStartup
namespace skylark
{
LPFN_DISCONNECTEX disconnectEx_ = nullptr;
LPFN_ACCEPTEX acceptEx_ = nullptr;
LPFN_CONNECTEX connectEx_ = nullptr;
char Socket::acceptBuf[64] = { 0, };


struct SocketInitializer
{
	SocketInitializer()
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
			crashAssert(false);

		//init ex functions
		SOCKET initEx = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
		crashAssert(initEx != INVALID_SOCKET);

		GUID guidDisconnectEx = WSAID_DISCONNECTEX;
		DWORD bytes = 0;

		if (SOCKET_ERROR == WSAIoctl(initEx, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidDisconnectEx, sizeof(GUID), &disconnectEx_, sizeof(LPFN_DISCONNECTEX), &bytes, NULL, NULL))
			crashAssert(false);

		GUID guidAcceptEx = WSAID_ACCEPTEX;
		if (SOCKET_ERROR == WSAIoctl(initEx, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidAcceptEx, sizeof(GUID), &acceptEx_, sizeof(LPFN_ACCEPTEX), &bytes, NULL, NULL))
			crashAssert(false);

		GUID guidConnectEx = WSAID_CONNECTEX;
		if (SOCKET_ERROR == WSAIoctl(initEx, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidConnectEx, sizeof(GUID), &connectEx_, sizeof(LPFN_CONNECTEX), &bytes, NULL, NULL))
			crashAssert(false);
	}

	~SocketInitializer()
	{
		WSACleanup();
	}
};

SocketInitializer initializer;

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

	crashAssert(socket != INVALID_SOCKET);
}

Socket::~Socket()
{
	closesocket(socket);
}

bool Socket::reuseAddr(bool reuse)
{
	int opt = reuse ? 1 : 0;

	return SOCKET_ERROR != setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int));
}

bool Socket::setLinger(bool lingerOn, int lingerTime)
{
	LINGER lingerOption;
	lingerOption.l_onoff = lingerOn ? 1 : 0;
	lingerOption.l_linger = lingerTime;

	return SOCKET_ERROR != setsockopt(socket, SOL_SOCKET, SO_LINGER, (const char*)&lingerOption, sizeof(LINGER));
}

bool Socket::updateAcceptContext(Socket * s)
{
	return SOCKET_ERROR != setsockopt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&s->socket, sizeof(SOCKET));
}

bool Socket::setNodelay(bool nodelay)
{
	int opt = nodelay ? 1 : 0;
	return SOCKET_ERROR != setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int));
}

bool Socket::setReceiveBufferSize(int size)
{
	return SOCKET_ERROR != setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(int));
}

bool Socket::completeTo(CompletionPort * port)
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

	return SOCKET_ERROR != ::bind(socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
}

bool Socket::listen()
{
	return SOCKET_ERROR != ::listen(socket, SOMAXCONN);
}
bool Socket::disconnect(Context * context)
{
	Overlapped* overlapped = new Overlapped(context);

	if (FALSE == disconnectEx_(socket, (LPOVERLAPPED)overlapped, TF_REUSE_SOCKET, 0))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			delete overlapped;

			return false;
		}
	}

	return true;
}
bool Socket::accept(Socket * listenSocket, Context * context)
{
	DWORD bytes = 0;
	Overlapped* overlapped = new Overlapped(context);

	if (FALSE == acceptEx_(listenSocket->socket, socket, acceptBuf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPOVERLAPPED)overlapped))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			delete overlapped;

			return false;
		}
	}
	return true;
}
bool Socket::connect(const std::string & addr, std::uint16_t port, Context * context)
{
	SOCKADDR_IN serverSockAddr = { 0, };

	serverSockAddr.sin_port = htons(port);
	serverSockAddr.sin_family = AF_INET;
	serverSockAddr.sin_addr.s_addr = inet_addr(addr.c_str());

	Overlapped* overlapped = new Overlapped(context);

	if (FALSE == connectEx_(socket, (sockaddr*)&serverSockAddr, sizeof(SOCKADDR_IN),
		nullptr, 0, nullptr, (LPOVERLAPPED)overlapped))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			delete overlapped;

			return false;
		}
	}

	return true;
}
bool Socket::recv(Context * context, WSABUF & buf)
{
	DWORD recvBytes = 0;
	DWORD flags = 0;

	Overlapped* overlapped = new skylark::Overlapped(context);
	if (SOCKET_ERROR == WSARecv(socket, &buf, 1, &recvBytes, &flags, (LPOVERLAPPED)overlapped, nullptr))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			delete overlapped;
			return false;
		}
	}

	return true;
}

bool Socket::send(Context * context, WSABUF & buf)
{
	Overlapped* overlapped = new Overlapped(context);
	DWORD sendbytes = 0;
	DWORD flags = 0;

	if (SOCKET_ERROR == WSASend(socket, &buf, 1, &sendbytes, flags, (LPOVERLAPPED)overlapped, nullptr))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			delete overlapped;

			return false;
		}
	}

	return true;
}
}