#pragma once


namespace skylark
{
	struct Context;

	class Port
	{
	public:
		Port(int timeout_);
		~Port();

		void job();
		bool bind(SOCKET socket);

		bool take(Context* context, int key);
	private:
		HANDLE completionPort;
		int timeout;
	};

	bool postContext(Port* port, Context* context, int key);
}