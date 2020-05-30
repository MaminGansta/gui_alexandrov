#include "errors.h"

namespace gui
{

	// global
	std::vector<int> error_list = std::vector<int>();


	int GetLastErrorCode()
	{
		return error_list.back();
	}


	const wchar_t* GetLastErrorW()
	{
		switch (GetLastErrorCode())
		{

			// file io
			case 0: return L"Error: File can't be opened\r\n";
			case 1: return L"Error: Read failled\r\n";
			case 2: return L"Error: Write failled\r\n";

			// console
			case 3: return L"Error: AllocConsole failled\r\n";
			case 4: return L"Error: Could not set control handler\r\n";
			case 5: return L"Error: Console write failled\r\n";

			// window (for more information call winapi function GetLastError())
			case 6: return L"Error: Cannot register window class\r\n";
			case 7: return L"Error: Cannot create window\r\n";

			// image
			case 8: return L"Error: Cannot open image\r\n";

			default:
				return L"Error: Error code undefined\r\n";
		}
	}



	const char* GetLastErrorA()
	{
		switch (GetLastErrorCode())
		{

			// file io
			case 0: return "Error: File can't be opened\r\n";
			case 1: return "Error: Read failled\r\n";
			case 2: return "Error: Write failled\r\n";

			// console
			case 3: return "Error: AllocConsole failled\r\n";
			case 4: return "Error: Could not set control handler\r\n";
			case 5: return "Error: Console write failled\r\n";

			// window (for more information call winapi function GetLastError())
			case 6: return "Error: Cannot register window class\r\n";
			case 7: return "Error: Cannot create window\r\n";

			// image
			case 8: return "Error: Cannot open image\r\n";

			default:
				return "Error: Error code undefined\r\n";
		}
	}

}