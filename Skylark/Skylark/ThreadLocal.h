#pragma once

namespace skylark
{
	enum class ThreadType
	{
		MAIN,
		IO_WORKER,
		DB_WORKER
	};

	enum COMPLETION_KEY_TYPE
	{
		NONE,
		DB_REQUEST,
		DB_RESULT
	};

	class LockOrderChecker;

	namespace TLS
	{
		extern thread_local int threadType;
		extern thread_local int workerThreadId;
		extern thread_local LockOrderChecker* lockOrderChecker;
	}
}