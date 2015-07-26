#pragma once
#include "Skylark.h"
#include <vector>

class EchoApplication : public skylark::Application
{
public:
	EchoApplication(int threadNum_);
	~EchoApplication();

	virtual int run();
	virtual bool init();
	
private:
	skylark::CompletionPort* mainPort;
	int threadNum;
	skylark::Socket* listen;
	std::vector<skylark::IOThread*> threads;
};