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
		UdpContext(SOCKADDR_IN address_);
		UdpContext(const std::string& addr, std::uint16_t port);
		virtual ~UdpContext() = default;

		SOCKADDR_IN address;
	};
	
	class Session;
	class Socket;

	struct FunctionContext : Context
	{
		FunctionContext(std::function<bool()> func_, std::function<bool()> failFunc_ = nullptr)
			: func(func_), failFunc(failFunc_) {}
		~FunctionContext() override = default;

		bool onComplete(int transferred, int key) override;

		bool onFailure() override;

		std::function<bool()> func = nullptr;
		std::function<bool()> failFunc = nullptr;
	};

	struct AcceptContext : Context
	{
		AcceptContext(Session* session_, Socket* listen_) : session(session_), listen(listen_) {}
		~AcceptContext() override = default;

		bool onComplete(int transferred, int key) override;

		bool onFailure() override;

		Session* session;
		Socket* listen;
	};

	struct PreRecvContext : Context
	{
		PreRecvContext(Session* session_)
			:session(session_) {}
		~PreRecvContext() override = default;

		bool onComplete(int transferred, int key) override;
		
		bool onFailure() override;

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
			:session(session_), reason(reason_) {}
		~DisconnectContext() override = default;

		bool onComplete(int transferred, int key) override;

		bool onFailure() override
		{
			return true;
		}

		Session* session;
		int reason;
	};

	struct Overlapped
	{
		Overlapped(Context* context_) : context(context_)
		{
			//init overlapped
			memset(&overlapped, 0, sizeof(OVERLAPPED));
		}

		~Overlapped()
		{
			delete context;
			context = nullptr;
		}

		OVERLAPPED overlapped;
		Context* context = nullptr;
	};
}