#pragma once

namespace skylark
{
	class CompletionPort;

	class IOThread
	{
	public:
		explicit IOThread(int id_, CompletionPort* port_, std::function<void()> customFunc_);
		explicit IOThread(int id_, CompletionPort* port_);
		virtual ~IOThread();

		virtual void ioJob();
		virtual void sendJob();
		virtual void customJob();
		virtual void init();

		virtual int wait();
		
		int getId();

	private:
		int id;
		CompletionPort* port;
		std::thread t;
		std::function<void()> customFunc;
	};
}