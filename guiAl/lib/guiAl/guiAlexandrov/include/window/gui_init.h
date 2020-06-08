#pragma once

#include "libs/time.h"
#include "libs/thread_pool.h"
#include <Windows.h>

namespace gui
{
	// global instange
	extern HINSTANCE _hInst;

	void init(HINSTANCE hInst, int max_threads = MAX_THREADS);

}
