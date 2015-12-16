#pragma once

namespace skylark
{

class Session;

struct UdpContext final : Context, ObjectPool<UdpContext>
{
	UdpContext();
	UdpContext(SOCKADDR_IN address_);
	UdpContext(const std::string& addr, std::uint16_t port);
	virtual ~UdpContext() = default;

	SOCKADDR_IN mAddress;
};

struct FunctionContext final : Context, ObjectPool<FunctionContext>
{
	FunctionContext(std::function<bool()> func, std::function<void()> failFunc = nullptr)
		: mFunc(func), mFailFunc(failFunc) {}
	~FunctionContext() override = default;

	bool onComplete(int transferred, int key) override;
	void onFailure() override;

	std::function<bool()> mFunc = nullptr;
	std::function<void()> mFailFunc = nullptr;
};

struct AcceptContext final : Context, ObjectPool<AcceptContext>
{
	AcceptContext(Session* session, Socket* listen) : mSession(session), mListen(listen) {}
	~AcceptContext() override = default;

	bool onComplete(int transferred, int key) override;

	Session* mSession;
	Socket* mListen;
};

struct PreRecvContext final : Context, ObjectPool<PreRecvContext>
{
	PreRecvContext(Session* session)
		:mSession(session) {}
	~PreRecvContext() override = default;

	bool onComplete(int transferred, int key) override;

	Session* mSession;
	WSABUF mWsabuf;
};

struct RecvContext final : Context, ObjectPool<RecvContext>
{
	RecvContext(Session* session)
		:mSession(session) {}
	~RecvContext() override = default;

	bool onComplete(int transferred, int key) override;
	void onFailure() override;

	Session* mSession;
	WSABUF mWsabuf;
};

struct SendContext final : Context, ObjectPool<SendContext>
{
	SendContext(Session* session)
		:mSession(session) {}
	~SendContext() override = default;

	bool onComplete(int transferred, int key) override;

	void onFailure() override;

	Session* mSession;
	WSABUF mWsabuf;
};

struct DisconnectContext final : Context, ObjectPool<DisconnectContext>
{
	DisconnectContext(Session* session, int reason)
		:mSession(session), mReason(reason) {}
	~DisconnectContext() override = default;

	bool onComplete(int transferred, int key) override;

	Session* mSession;
	int mReason;
};

}