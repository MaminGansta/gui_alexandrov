#pragma once

#include "libs/time.h"
#include "libs/thread_pool.h"
#include <Windows.h>

namespace gui
{
	// globals
	extern HINSTANCE _hInst;
	extern ThreadPool thread_pool;

	void init(HINSTANCE hInst);
}
