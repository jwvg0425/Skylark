#pragma once


namespace skylark
{
	class Port
	{
	public:
		bool bind(SOCKET socket);
	private:
		HANDLE completionPort;
	};
}