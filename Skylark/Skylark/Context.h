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
	
	class Session;
	class Socket;

	struct AcceptContext : Context
	{
		AcceptContext(Session* session_, Socket* listen_) : session(session_), listen(listen_) {}
		virtual ~AcceptContext() = default;

		virtual bool onComplete(int transferred, int key);

		virtual bool onFailure()
		{
			return false;
		}

		Session* session;
		Socket* listen;
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