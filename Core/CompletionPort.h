#pragma once

namespace skylark
{
struct Context;

class CompletionPort
{
public:
	CompletionPort(int timeout);
	~CompletionPort();

	void job() const;
	bool bind(SOCKET socket);

	bool take(Context* context, int key);

private:
	HANDLE mCompletionPort;
	int mTimeout;
};

bool postContext(CompletionPort* port, Context* context, int key);
}