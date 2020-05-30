#pragma once

#include <Windows.h>


namespace gui
{
	// return full path to file
	bool open_file_window(wchar_t* filename, int max_size, HWND parent, wchar_t* filter = NULL);


	// return full path with new file name 
	bool save_file_window(wchar_t* dirname, int max_size, HWND parent, wchar_t* filter = NULL);
}