#pragma once

namespace skylark
{
	class Application
	{
	public:
		Application();
		~Application();

		virtual int run();
		virtual void init() = 0;

	private:
	};
}