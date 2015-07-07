#pragma once

namespace skylark
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		virtual int run();
		virtual bool init() = 0;

	private:
	};
}