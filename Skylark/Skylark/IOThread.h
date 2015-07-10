#pragma once

namespace skylark
{
	class Port;

	class IOThread
	{
	public:
		explicit IOThread(int id_, Port* port_, std::function<void()> customFunc_);
		explicit IOThread(int id_, Port* port_);
		virtual ~IOThread();

		virtual void ioJob();
		virtual void sendJob();
		virtual void customJob();
		virtual void init();

		virtual int wait();

	private:
		int id;
		Port* port;
		std::thread t;
		std::function<void()> customFunc;
	};
}