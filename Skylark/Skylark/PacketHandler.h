#pragma once
#include "CircularBuffer.h"
#include <functional>
#include <map>

namespace skylark
{

template <typename Header, int MAX_HANDLER_SIZE>
class PacketHandler
{
public:
	using Selector = std::function<int(Header)>;

	class Handler
	{
	public:
		virtual ~Handler() = default;

		virtual bool handle(Session* session) = 0;
	};

	template<typename Packet>
	class RealHandler : public Handler
	{
	public:
		RealHandler(std::function<void(const Packet&)> handler_)
			: handler(handler_) {}

		bool handle(Session* session) override
		{
			Packet packet;

			if (!session->peekPacket(packet))
			{
				return false;
			}

			if (!session->parsePacket(packet))
			{
				return false;
			}

			handler(packet);

			return true;
		}

	private:
		std::function<void(const Packet&)> handler;
	};

	PacketHandler()
	{
		for (int i = 0; i < MAX_HANDLER_SIZE; i++)
		{
			handlers[i] = nullptr;
		}
	}

	~PacketHandler()
	{
		for (int i = 0; i < MAX_HANDLER_SIZE; i++)
		{
			if (handlers[i] != nullptr)
				delete handlers[i];
		}
	}

	void registerSelector(Selector selector_)
	{
		selector = selector_;
	}

	template<typename Packet>
	void registerHandler(int type, std::function<void(const Packet&)> handler)
	{
		handlers[type] = new RealHandler<Packet>(handler);
	}

	void packetHandle(Session* session)
	{
		while (true)
		{
			Header header;

			if (!session->peekPacket(header))
				return;

			int idx = selector(header);
			
			if (idx <0 || idx > MAX_HANDLER_SIZE)
				return;

			if (!handlers[idx]->handle(session))
				return;
		}
	}

private:
	Selector selector;
	Handler* handlers[MAX_HANDLER_SIZE];

};

}