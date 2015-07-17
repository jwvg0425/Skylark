#pragma once
#include "Lock.h"

namespace skylark
{
	class CompletionPort;
	struct Context;

	enum class ConnectType
	{
		TCP,
		UDP
	};

	extern LPFN_DISCONNECTEX disconnectEx_;
	extern LPFN_ACCEPTEX acceptEx_;
	extern LPFN_CONNECTEX connectEx_;

	class Socket
	{
	public:
		Socket(ConnectType type);
		virtual ~Socket();

		bool reuseAddr(bool reuse);
		bool setLinger(bool lingerOn, int lingerTime);
		bool updateAcceptContext(Socket* s);
		bool setNodelay(bool nodelay);
		bool setReceiveBufferSize(int size);

		bool completeTo(CompletionPort* port);

		bool bind(const std::string& addr, std::uint16_t port);
		bool listen();

		template<typename C>
		bool disconnectEx(C* context)
		{
			static_assert(std::is_base_of<Context, C>::value, "context must be Context's derived type.");

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
		
		template<typename C>
		bool acceptEx(Socket* listenSocket, C* context)
		{
			static_assert(std::is_base_of<Context, C>::value, "context must be Context's derived type.");

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

		template<typename C>
		bool connectEx(const std::string& addr, std::uint16_t port, C* context)
		{
			static_assert(std::is_base_of(C, Context));

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

		template<typename C>
		bool recv(C* context, WSABUF& buf)
		{
			static_assert(std::is_base_of<Context, C>::value, "context must be Context's derived type.");

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

		template<typename C>
		bool recvFrom(C* context, WSABUF& buf)
		{
			static_assert(std::is_base_of<UdpContext, C>::value, "context must be Context's derived type.");

			DWORD recvBytes = 0;
			DWORD flags = 0;
			int size = sizeof(SOCKADDR_IN);

			Overlapped* overlapped = new skylark::Overlapped(context);
			if (SOCKET_ERROR == WSARecvFrom(socket, &buf, 1, &recvBytes, &flags, (sockaddr*)&context->address, &size, (LPOVERLAPPED)overlapped, nullptr))
			{
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					delete overlapped;
					return false;
				}
			}

			return true;
		}

		template<typename C>
		bool sendTo(C* context, WSABUF& buf)
		{
			static_assert(std::is_base_of<UdpContext, C>::value, "context must be UdpContext's derived type.");

			Overlapped* overlapped = new Overlapped(context);
			DWORD sendbytes = 0;
			DWORD flags = 0;
			int size = sizeof(SOCKADDR_IN);

			if (SOCKET_ERROR == WSASendTo(socket, &buf, 1, &sendbytes, flags, (sockaddr*)&context->address, size, (LPOVERLAPPED)overlapped, nullptr))
			{
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					delete overlapped;

					return false;
				}
			}

			return true;
		}

		template<typename C>
		bool send(C* context, WSABUF& buf)
		{
			static_assert(std::is_base_of<Context, C>::value, "context must be UdpContext's derived type.");

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
	
	private:
		static char acceptBuf[64];
		SOCKET socket;
		bool bindToPort = false;
	};
}