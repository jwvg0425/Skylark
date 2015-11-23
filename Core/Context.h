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

struct Overlapped
{
	Overlapped(Context* context) : mContext(context)
	{
		//init overlapped
		memset(&mOverlapped, 0, sizeof(OVERLAPPED));
	}

	~Overlapped()
	{
		delete mContext;
		mContext = nullptr;
	}

	OVERLAPPED mOverlapped;
	Context* mContext = nullptr;
};
}