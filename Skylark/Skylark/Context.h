#pragma once

namespace skylark
{
	struct Context
	{
		Context() = default;
		virtual ~Context() = default;

		virtual bool onComplete(int transferred, int key) = 0;
		
		//call when onComplete return false
		virtual bool onFailure() = 0;
	};

	struct UdpContext : Context
	{
		UdpContext();
		UdpContext(const std::string& addr, std::uint16_t port);
		virtual ~UdpContext() = default;

		SOCKADDR_IN address;
	};
	
	class Session;
	class Socket;

	struct AcceptContext : Context
	{
		AcceptContext(Session* session_, Socket* listen_) : session(session_), listen(listen_) {}
		~AcceptContext() override = default;

		bool onComplete(int transferred, int key) override;

		bool onFailure() override
		{
			return false;
		}

		Session* session;
		Socket* listen;
	};

	struct PreRecvContext : Context
	{
		PreRecvContext(Session* session_)
			:session(session_) {}
		~PreRecvContext() override = default;

		bool onComplete(int transferred, int key) override;
		
		bool onFailure() override
		{
			return false;
		}

		Session* session;
		WSABUF wsabuf;
	};

	struct RecvContext : Context
	{
		RecvContext(Session* session_)
			:session(session_) {}
		~RecvContext() override = default;

		bool onComplete(int transferred, int key) override;

		bool onFailure() override;

		Session* session;
		WSABUF wsabuf;
	};

	struct SendContext : Context
	{
		SendContext(Session* session_)
			:session(session_) {}
		~SendContext() override = default;

		bool onComplete(int transferred, int key) override;

		bool onFailure() override;

		Session* session;
		WSABUF wsabuf;
	};

	struct DisconnectContext : Context
	{
		DisconnectContext(Session* session_, int reason_)
			:session(session), reason(reason_) {}
		~DisconnectContext() override = default;

		bool onComplete(int transferred, int key) override;

		bool onFailure() override
		{
			return false;
		}

		Session* session;
		int reason;
	};

	struct Overlapped : OVERLAPPED
	{
		Overlapped(Context* context_) : context(context_)
		{
			//init overlapped
			memset(this, 0, sizeof(OVERLAPPED));
		}

		~Overlapped()
		{
			delete context;
		}

		Context* context;
	};
}