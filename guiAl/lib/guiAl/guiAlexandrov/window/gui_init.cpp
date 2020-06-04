
#include "window/gui_init.h"


namespace gui
{
	// global
	HINSTANCE _hInst;


	void init(HINSTANCE hInstance)
	{
		_hInst = hInstance;
		__init_time = high_resolution_clock::now();
	}

}