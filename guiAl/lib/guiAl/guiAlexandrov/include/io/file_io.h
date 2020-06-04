#pragma once

#include <Windows.h>
#include "error_handler/errors.h"


namespace gui
{

	// Read

	/*
		Return heap allocated buffer with file data.

		if function return NULL, call GetLastError()
	*/
	
	// wcahr_t
	wchar_t* read_file(const wchar_t* file_name, int& len);

	// char
	char* read_file(const char* file_name, int& len);



	// Write
	
	/*
		return amount of chars that was written

		if return 0, call GetLastErrorFunction
	*/

	// wchar_t
	int write_file(const wchar_t* file_name, wchar_t* data, int len);


	// char
	int write_file(const char* file_name, char* data, int len);
}