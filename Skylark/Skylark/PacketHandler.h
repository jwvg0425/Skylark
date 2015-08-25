#pragma once
#include "CircularBuffer.h"
#include <functional>
#include <map>

namespace skylark
{

//for session
template <typename Header, typename User, int MAX_HANDLER_SIZE>
class PacketHandler
{
public:
	using Selector = std::function<int(Header)>;

	PacketHandler(User* user_)
		:user(user_)
	{
		for (int i = 0; i < MAX_HANDLER_SIZE; i++)
		{
			handlers[i] = nullptr;
		}
	}

	~PacketHandler() = default;

	void registerSelector(Selector selector_)
	{
		selector = selector_;
	}

	template<typename Enum, typename Packet>
	void registerHandler(Enum type, void (User::*handler)(const Packet&))
	{
		handlers[static_cast<int>(type)] = [this, handler]() -> bool
		{
			Packet p;

			if (!user->parsePacket(p))
			{
				return false;
			}

			(user->*handler)(p);

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
	User* user;
	Selector selector;
	std::function<bool()> handlers[MAX_HANDLER_SIZE];

};

}