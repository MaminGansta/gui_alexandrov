
/*
	Return heap allocated buffer with file data.

	if function return NULL, call GetLastError()
*/

template <typename T>
T* read_file(T* file_name , int& len)
{
	HANDLE hFile;

	hFile = CreateFile(file_name,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		gui::error_list.push_back(0);
		return  NULL;
	}

	len = GetFileSize(hFile, NULL);
	DWORD nBytesRead;
	BOOL bResult;
	T* data = (T*)malloc(sizeof(T) * len);

	bResult = ReadFile(hFile, data, len * sizeof(T), &nBytesRead, NULL);

	if (!bResult)
	{
		gui::error_list.push_back(1);
		return NULL;
	}

	CloseHandle(hFile);
	return data;
}


/*
	return amount of chars that was written

	if return 0, call GetLastErrorFunction
*/

template <typename T>
int write_file(T* file_name, T* data, int len)
{
	HANDLE hFile;

	hFile = CreateFile(file_name,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		gui::error_list.push_back(0);
		return 0;
	}

	DWORD nBytesWrited;
	BOOL bResult;
	bResult = WriteFile(hFile, data, len * sizeof(T), &nBytesWrited, NULL);

	if (!bResult)
	{
		gui::error_list.push_back(2);
		return 0;
	}

	CloseHandle(hFile);
	return nBytesWrited / sizeof(T);
}