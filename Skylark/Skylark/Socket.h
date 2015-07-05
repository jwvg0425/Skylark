#pragma once

namespace skylark
{
	class Port;
	class Context;

	enum class ConnectType
	{
		TCP,
		UDP
	};

	enum class SocketOption
	{
		REUSE_ADDR
	};

	class Socket
	{
	public:
		Socket(ConnectType type);
		~Socket();

		void setOption(SocketOption option);
		bool completeTo(Port* port);

		bool bind(const std::string& addr, std::uint16_t port);
		bool listen();

		template<typename C>
		bool disconnectEx(C* context)
		{
			static_assert(std::is_base_of(C, Context));

			return false;
		}
		
		template<typename C>
		bool acceptEx(Socket* listenSocket, C* context)
		{
			static_assert(std::is_base_of(C, Context));

			return false;
		}

		template<typename C>
		bool connectEx(const std::string& addr, std::uint16_t port, C* context)
		{
			static_assert(std::is_base_of(C, Context));

			return false;
		}
	
	private:
		SOCKET socket;
		bool bindToPort = false;
	};
}