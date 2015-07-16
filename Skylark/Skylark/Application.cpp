#include "stdafx.h"
#include "Application.h"
#include "MemoryPool.h"

skylark::Application::Application()
{
	//global manager init
	MemoryPool::GetInstance();
}

skylark::Application::~Application()
{

}

int skylark::Application::run()
{
	return 0;
}
