
#include "log.h"


namespace gui
{

	template <size_t buffer_size = 128>
	void doutput(const char* format, ...)
	{
#if _DEBUG
		char log[buffer_size];
		va_list args;
		va_start(args, format);
		vsprintf_s(log, format, args);
		OutputDebugStringA(log);
		va_end(args);
#endif
	}

	template <size_t buffer_size = 128>
	void doutput(const wchar_t* format, ...)
	{
#if _DEBUG
		wchar_t log[buffer_size];
		va_list args;
		va_start(args, format);
		vswprintf_s(log, format, args);
		OutputDebugStringW(log);
		va_end(args);
#endif
	}


	template <size_t buffer_size = 128>
	void output(const char* format, ...)
	{
		char log[buffer_size];
		va_list args;
		va_start(args, format);
		vsprintf_s(log, format, args);
		OutputDebugStringA(log);
		va_end(args);
	}

	template <size_t buffer_size = 128>
	void output(const wchar_t* format, ...)
	{
		wchar_t log[buffer_size];
		va_list args;
		va_start(args, format);
		vswprintf_s(log, format, args);
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
		int create_console(BOOL(*WINAPI console_callback)(DWORD fdwCtrlType))
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
				gui::error_list.push_back(5);
				return 0;
			}

			return written;
		}


		template <size_t buffer_size = 128>
		int printf(const wchar_t* format, ...)
		{
			if (!console)
			{
				if (!create_console())
					return 0;
			}

			wchar_t log[buffer_size];
			va_list args;
			va_start(args, format);

			// write to the buffer
			int length = vswprintf_s(log, format, args);

			// print buffer to the console
			DWORD written = 0;
			BOOL res = WriteConsoleW(console, log, length, &written, NULL);

			va_end(args);

			if (!res)
			{
				gui::error_list.push_back(5);
				return 0;
			}

			return written;
		}

	}

}