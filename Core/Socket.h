#pragma once
#include "Lock.h"

namespace skylark
{
class CompletionPort;
struct Context;

enum class ConnectType
{
	TCP,
	UDP
};

extern LPFN_DISCONNECTEX disconnectEx_;
extern LPFN_ACCEPTEX acceptEx_;
extern LPFN_CONNECTEX connectEx_;

class Socket
{
public:
	Socket(ConnectType type);
	virtual ~Socket();

	bool reuseAddr(bool reuse);
	bool setLinger(bool lingerOn, int lingerTime);
	bool updateAcceptContext(Socket* s);
	bool setNodelay(bool nodelay);
	bool setReceiveBufferSize(int size);

	bool completeTo(CompletionPort* port);

	bool bind(const std::string& addr, std::uint16_t port);
	bool listen();

	bool disconnect(Context* context);
	bool accept(Socket* listenSocket, Context* context);
	bool connect(const std::string& addr, std::uint16_t port, Context* context);

	bool recv(Context* context, WSABUF& buf);
	bool send(Context* context, WSABUF& buf);

private:
	static char acceptBuf[64];
	SOCKET socket;
	bool bindToPort = false;
};
}