#pragma once
#include "CircularBuffer.h"
#include <functional>
#include <map>

namespace skylark
{

//for session
template <typename Header, int MAX_HANDLER_SIZE>
class SessionPacketHandler
{
public:
	using Selector = std::function<int(Header)>;

	SessionPacketHandler(Session* session_)
		:session(session_)
	{
		for (int i = 0; i < MAX_HANDLER_SIZE; i++)
		{
			handlers[i] = nullptr;
		}
	}

	~SessionPacketHandler() = default;

	void registerSelector(Selector selector_)
	{
		selector = selector_;
	}

	template<typename Packet>
	void registerHandler(int type, std::function<void(const Packet&)> handler)
	{
		handlers[type] = [this, handler]() -> bool
		{
			Packet p;

			if (!session->parsePacket(p))
			{
				return false;
			}

			handler(p);

			return true;
		};
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

			CRASH_ASSERT(handlers[idx] != nullptr);

			if (!handlers[idx]())
				return;
		}
	}

private:
	Session* session;
	Selector selector;
	std::function<bool()> handlers[MAX_HANDLER_SIZE];

};

}