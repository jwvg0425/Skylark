#pragma once
#include "CircularBuffer.h"
#include "Lock.h"

namespace skylark
{
	class CompletionPort;
	class Socket;
	struct Context;

	class Session
	{
	public:
		Session(CompletionPort* port, std::size_t sendBufSize, std::size_t recvBufSize);
		virtual ~Session();

		bool disconnect(Context* context);
		bool accept(Socket* listen, Context* context);

		bool preRecv();
		bool recv();

		virtual bool onAccept();
		virtual bool onRead();
		virtual bool onSend();
		virtual bool onDisconnect(int reason);

		bool acceptCompletion(Socket* listen);
		bool sendCompletion(DWORD transferred);
		bool recvCompletion(DWORD transferred);
		bool disconnectCompletion(int reason);

		void addRef();
		void releaseRef();

		virtual void onRelease();

		virtual bool send(char* packet, std::size_t len);
		virtual bool flushSend();

		template<typename Packet>
		bool peekPacket(Packet& p)
		{
			if (recvBuffer.getStoredSize() < sizeof(p))
			{
				return false;
			}

			return recvBuffer.peek((char*)&p, sizeof(Packet));
		}

		template<typename Packet>
		bool parsePacket(Packet& p)
		{
			if (!recvBuffer.read((char*)&p, sizeof(Packet)))
			{
				return false;
			}

			return true;
		}

		template<typename Packet>
		bool sendPacket(const Packet& p)
		{
			if (!send((char*)&p, sizeof(Packet)))
			{
				return false;
			}

			return true;
		}

		bool isConnected();

		int getRefCount() { return refCount; }

	protected:
		CompletionPort* port;
		Socket* socket = nullptr;
		CircularBuffer sendBuffer;
		CircularBuffer recvBuffer;
		Lock sendLock;
		int sendPendingCount = 0;
		std::atomic<bool> connected;
		std::atomic<int> refCount;

	};
}