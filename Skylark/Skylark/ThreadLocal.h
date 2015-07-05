#pragma once

namespace skylark
{
	class LockOrderChecker;

	namespace TLS
	{
		extern thread_local LockOrderChecker* lockOrderChecker;
	}
}