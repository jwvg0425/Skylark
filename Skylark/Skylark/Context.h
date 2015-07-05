#pragma once

namespace skylark
{
	struct Context
	{
		Context(int type_) : type(type_) {}
		~Context() = default;

		virtual bool onComplete(int transferred, int key) = 0;
		
		//call when onComplete return false
		virtual bool onFailure() = 0;

		OVERLAPPED overlapped;
		WSABUF wsabuf;
		int type;
	};
}