#pragma once
#include "CircularBuffer.h"
#include "Lock.h"

namespace skylark
{
	class CompletionPort;
	class Socket;

	class Session
	{
	public:
		Session(CompletionPort* port, std::size_t sendBufSize, std::size_t recvBufSize);
		virtual ~Session();

		template<typename C>
		bool disconnect(C* context)
		{
			static_assert(std::is_base_of<Context, C>::value, "context must be Context's derived type.");

			return socket->disconnectEx(context);
		}

		template<typename C>
		bool accept(Socket* listen, C* context)
		{
			static_assert(std::is_base_of<Context ,C>::value, "context must be Context's derived type.");

			return socket->acceptEx(listen, context);
		}

		bool preRecv();
		bool recv();

		virtual bool onAccept();
		virtual bool onRead();
		virtual bool onSend();
		virtual bool onDisconnect(int reason);

		bool acceptCompletion(Socket* listen);
		bool sendCompletion(DWORD transferred);
		bool recvCompletion(DWORD transferred);

		virtual bool send(char* packet, std::size_t len);
		virtual bool flushSend();

		bool isConnected();

	protected:
		CompletionPort* port;
		Socket* socket = nullptr;
		CircularBuffer sendBuffer;
		CircularBuffer recvBuffer;
		Lock sendLock;
		int sendPendingCount = 0;
		std::atomic<bool> connected;

	};
}