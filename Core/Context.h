#pragma once
#include "ObjectPool.h"

namespace skylark
{
struct Context
{
	Context() = default;
	virtual ~Context() = default;

	virtual bool onComplete(int transferred, int key) = 0;

	//call when onComplete return false
	virtual void onFailure() {}
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