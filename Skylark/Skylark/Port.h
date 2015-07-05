#pragma once


namespace skylark
{
	struct Context;

	class Port
	{
	public:
		Port();
		~Port();

		void job(int timeout);
		bool bind(SOCKET socket);

		bool take(Context* context, int key);
	private:
		HANDLE completionPort;
	};

	bool postContext(Port* port, Context* context, int key);
}