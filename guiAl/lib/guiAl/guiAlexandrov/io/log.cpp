
#include "io/log.h"


namespace gui
{

	void output(const char* format, ...)
	{
		va_list args;
		va_start(args, format);

		int buffer_size = _vscprintf(format, args) + 1;
		char* log = (char*)_malloca(buffer_size);

		vsprintf_s(log, buffer_size, format, args);
		OutputDebugStringA(log);
		va_end(args);
	}

	void output(const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);

		int buffer_size = _vscwprintf(format, args) + 1;
		wchar_t* log = (wchar_t*)_malloca(buffer_size * sizeof(wchar_t));

		vswprintf_s(log, buffer_size, format, args);
		OutputDebugStringW(log);
		va_end(args);
	}




	void str_output(const wchar_t* str)
	{
		OutputDebugStringW(str);
	}

	void str_output(const char* str)
	{
		OutputDebugStringA(str);
	}





	namespace Log
	{
		// global
		std::vector<wchar_t> al_log;

		void add_log(wchar_t* log, int lenght)
		{
			for (int i = 0; i < lenght; i++)
				al_log.push_back(log[i]);
		}


		void add_log(const std::wstring& log)
		{
			for (int i = 0; i < log.size(); i++)
				al_log.push_back(log[i]);
		}


		bool dumb_log(wchar_t* filename)
		{
			return write_file(filename, al_log.data(), al_log.size());
		}

	}



	namespace console
	{
		// global
		HANDLE console = 0;

		// default console event handler
		BOOL WINAPI console_callback_plug(DWORD fdwCtrlType)
		{
			return FALSE;
		}

		/*
			Posible to rewrite console event handler
			but not possible to prevent window closing
			after console colse (winapi doesn't support it)
		*/
		int create_console(BOOL(WINAPI *console_callback)(DWORD fdwCtrlType))
		{
			if (console) return 1;

			if (!AllocConsole())
			{
				gui::error_list.push_back(3);
				return 0;
			}

			if (!SetConsoleCtrlHandler(console_callback, TRUE))
			{
				gui::error_list.push_back(4);
				return 0;
			}

			console = GetStdHandle(STD_OUTPUT_HANDLE);
			return 1;
		}


		int detach_console()
		{
			return (int)FreeConsole;
		}


		int print(const wchar_t* text, int length)
		{
			if (!console)
			{
				if (!create_console())
					return 0;
			}

			DWORD written = 0;
			BOOL res = WriteConsoleW(console, text, length, &written, NULL);

			if (!res)
			{
				error_list.push_back(5);
				return 0;
			}

			return written;
		}



		int printf(const char* format, ...)
		{
			if (!console)
			{
				if (!create_console())
					return 0;
			}

			va_list args;
			va_start(args, format);

			// allocate buffer
			int buffer_size = _vscprintf(format, args) + 1;
			char* log = (char*)_malloca(buffer_size);

			// write to the buffer
			int length = vsprintf_s(log, buffer_size, format, args);

			// print buffer to the console
			DWORD written = 0;
			BOOL res = WriteConsoleA(console, log, length, &written, NULL);

			va_end(args);

			if (!res)
			{
				error_list.push_back(5);
				return 0;
			}

			return written;
		}


		int printf(const wchar_t* format, ...)
		{
			if (!console)
			{
				if (!create_console())
					return 0;
			}

			va_list args;
			va_start(args, format);

			// allocate buffer
			int buffer_size = _vscwprintf(format, args) + 1;
			wchar_t* log = (wchar_t*)_malloca(buffer_size * sizeof(wchar_t));

			// write to the buffer
			int length = vswprintf_s(log, buffer_size, format, args);

			// print buffer to the console
			DWORD written = 0;
			BOOL res = WriteConsoleW(console, log, length, &written, NULL);

			va_end(args);

			if (!res)
			{
				error_list.push_back(5);
				return 0;
			}

			return written;
		}


	}
}