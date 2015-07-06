#pragma once

namespace skylark
{
	class Port;

	class IOThread
	{
	public:
		explicit IOThread(int id_, Port* port_, std::function<void()> customFunc_);
		~IOThread();

		virtual void sendJob();
		virtual void init();

		int run();

	private:
		int id;
		Port* port;
		std::function<void()> customFunc;
		std::thread t;
	};
}