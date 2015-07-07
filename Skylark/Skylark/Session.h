#pragma once
#include "CircularBuffer.h"
#include "Lock.h"

namespace skylark
{
	class Port;
	class Socket;

	class Session
	{
	public:
		Session(Port* port, std::size_t sendBufSize, std::size_t recvBufSize);
		virtual ~Session();

		void disconnect(int reason);

		template<typename C>
		bool accept(Socket* listen, C* context)
		{
			static_assert(std::is_base_of<Context ,C>::value, "context must be Context's derived type.");

			return socket->acceptEx(listen, context);
		}

		bool preRecv();
		bool recv();

		virtual bool onAccept(Socket* listen);
		virtual bool onRead();

		virtual bool send(std::int8_t* packet, std::size_t len);
		virtual bool flushSend();

	private:
		Port* port;
		Socket* socket = nullptr;
		CircularBuffer sendBuffer;
		CircularBuffer recvBuffer;
		Lock sendLock;
		int sendPendingCount = 0;

	};
}