#include "stdafx.h"
#include "Application.h"
#include "MemoryPool.h"
#include "Exception.h"

skylark::Application::Application()
{
	//for dump on crash
	SetUnhandledExceptionFilter(exceptionFilter);

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
