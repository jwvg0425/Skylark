#pragma once

namespace skylark
{
	struct Context
	{
		Context() = default;
		~Context() = default;

		virtual bool onComplete(int transferred, int key) = 0;
		
		//call when onComplete return false
		virtual bool onFailure() = 0;
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