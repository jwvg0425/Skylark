#pragma once


namespace skylark
{
	struct Context;

	class CompletionPort
	{
	public:
		CompletionPort(int timeout_);
		~CompletionPort();

		void job() const;
		bool bind(SOCKET socket);

		bool take(Context* context, int key);

		bool doLambda(std::function<bool()> func, std::function<bool()> failedFunc = nullptr);
	private:
		HANDLE completionPort;
		int timeout;
	};

	bool postContext(CompletionPort* port, Context* context, int key);
}