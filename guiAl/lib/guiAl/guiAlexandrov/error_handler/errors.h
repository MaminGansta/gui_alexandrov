#pragma once

#include <vector>

namespace gui
{
	extern std::vector<int> error_list;

	// return last error code
	int GetLastErrorCode();

	// return short description of error
	const wchar_t* GetLastErrorW();

	const char* GetLastErrorA();
}
