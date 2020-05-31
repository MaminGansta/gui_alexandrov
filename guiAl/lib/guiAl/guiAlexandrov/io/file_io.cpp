
#include "io/file_io.h"


namespace gui
{
	
	// Read

	// wcahr_t
	wchar_t* read_file(const wchar_t* file_name, int& len)
	{
		HANDLE hFile;

		hFile = CreateFileW(file_name,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);


		if (hFile == INVALID_HANDLE_VALUE)
		{
			error_list.push_back(0);
			return  NULL;
		}

		len = GetFileSize(hFile, NULL);
		DWORD nBytesRead;
		BOOL bResult;
		wchar_t* data = (wchar_t*)malloc(sizeof(wchar_t) * len);

		bResult = ReadFile(hFile, data, len * sizeof(wchar_t), &nBytesRead, NULL);

		if (!bResult)
		{
			error_list.push_back(1);
			return NULL;
		}

		CloseHandle(hFile);
		return data;
	}

	// char
	char* read_file(const char* file_name, int& len)
	{
		HANDLE hFile;

		hFile = CreateFileA(file_name,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);


		if (hFile == INVALID_HANDLE_VALUE)
		{
			error_list.push_back(0);
			return  NULL;
		}

		len = GetFileSize(hFile, NULL);
		DWORD nBytesRead;
		BOOL bResult;
		char* data = (char*)malloc(sizeof(char) * len);

		bResult = ReadFile(hFile, data, len * sizeof(char), &nBytesRead, NULL);

		if (!bResult)
		{
			error_list.push_back(1);
			return NULL;
		}

		CloseHandle(hFile);
		return data;
	}



	// Write

	// wchar_t
	int write_file(const wchar_t* file_name, wchar_t* data, int len)
	{
		HANDLE hFile;

		hFile = CreateFileW(file_name,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			error_list.push_back(0);
			return 0;
		}

		DWORD nBytesWrited;
		BOOL bResult;
		bResult = WriteFile(hFile, data, len * sizeof(wchar_t), &nBytesWrited, NULL);

		if (!bResult)
		{
			error_list.push_back(2);
			return 0;
		}

		CloseHandle(hFile);
		return nBytesWrited / sizeof(wchar_t);
	}


	// char
	int write_file(const char* file_name, char* data, int len)
	{
		HANDLE hFile;

		hFile = CreateFileA(file_name,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			error_list.push_back(0);
			return 0;
		}

		DWORD nBytesWrited;
		BOOL bResult;
		bResult = WriteFile(hFile, data, len, &nBytesWrited, NULL);

		if (!bResult)
		{
			error_list.push_back(2);
			return 0;
		}

		CloseHandle(hFile);
		return nBytesWrited;
	}

}