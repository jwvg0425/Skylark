#pragma once
#include "CircularBuffer.h"
#include <functional>
#include <map>

namespace skylark
{

template<typename T>
struct InitHandler
{
public:
	InitHandler()
	{
		T::initHandler();	}

	~InitHandler() = default;

private:
};

//for session
template <typename Header, typename User, int MAX_HANDLER_SIZE>
class PacketHandler
{
public:
	PacketHandler()
	{
		static InitHandler<User> init;
	}

	virtual ~PacketHandler() = default;

	virtual int select(Header header) = 0;

	template<typename Enum, typename Packet>
	static void registerHandler(Enum type, void (User::*handler)(const Packet&))
	{
		handlers[static_cast<int>(type)] = [handler](User* user) -> bool
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

	void packetHandle(User* user)
	{
		while (true)
		{
			Header header;

			if (!user->peekPacket(header))
				return;

			int idx = select(header);

			if (idx <0 || idx > MAX_HANDLER_SIZE)
				return;

			CRASH_ASSERT(handlers[idx] != nullptr);

			if (!handlers[idx](user))
				return;
		}
	}

private:
	static std::function<bool(User*)> handlers[MAX_HANDLER_SIZE];
};


template <typename Header, typename User, int MAX_HANDLER_SIZE>
std::function<bool(User*)> PacketHandler<Header, User, MAX_HANDLER_SIZE>::handlers[MAX_HANDLER_SIZE] = { nullptr, };

}