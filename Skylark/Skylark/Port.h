#pragma once


namespace skylark
{
	class Port
	{
	public:
		Port();
		~Port();

		bool bind(SOCKET socket);
	private:
		HANDLE completionPort;
	};
}