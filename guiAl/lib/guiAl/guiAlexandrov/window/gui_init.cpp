
#include "window/gui_init.h"


namespace gui
{
	// globals
	HINSTANCE _hInst;
	ThreadPool thread_pool;

	void init(HINSTANCE hInstance)
	{
		_hInst = hInstance;
		__init_time = high_resolution_clock::now();
	}

}