#include "EchoApplication.h"

int main()
{
	EchoApplication app(4);

	if (!app.init())
	{
		printf("app init error.");
		return -1;
	}

	return app.run();
}