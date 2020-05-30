#pragma once

#include <Windows.h>
#include <vector>
#include <string>

#include "file_io.h"
#include "../error_handler/errors.h"

/*
	_Print to visual studio output window:

		Template <buffer_size = 128>
		void output(const char/wchar* format, ...)  - print format string

		Template <buffer_size = 128>
		void doutput(const char/wchar* format, ...) - print format string only in debug mod

		void str_output(const wchar_t* str)			- print string



	_Print to console window:

	namespace console:
		int print(const wchar_t* text, int length) - Create console if it's not exist
													 and add new text to console

		Template <buffer_size = 128>
		int printf(const wchar_t* format, ...) - Create console if it's not exist
												 and add new formated text to console


	_Print to console window:

	namespace Log:
		void add_log(wchar_t* log, int lenght) - Add c string to log
		void add_log(const std::wstring& log)  - Add c++ string to log

		void dumb_log(wchar_t* filename) - write log to file
		void to_console()                - write log to console

*/


namespace gui
{


	template <size_t buffer_size = 128>
	void doutput(const char* format, ...);


	template <size_t buffer_size = 128>
	void doutput(const wchar_t* format, ...);



	template <size_t buffer_size = 128>
	void output(const char* format, ...);

	template <size_t buffer_size = 128>
	void output(const wchar_t* format, ...);



	void str_output(const wchar_t* str);

	void str_output(const char* str);




	namespace Log
	{
		std::vector<wchar_t> al_log;

		void add_log(wchar_t* log, int lenght);

		void add_log(const std::wstring& log);

		bool dumb_log(wchar_t* filename);
	}



	namespace console
	{
		HANDLE console = 0;


		// default console event handler
		BOOL WINAPI console_callback_plug(DWORD fdwCtrlType);

		/*
			Posible to rewrite console event handler
			but not possible to prevent window closing
			after console colse (winapi doesn't support it)
		*/
		int create_console(BOOL(*WINAPI console_callback)(DWORD fdwCtrlType) = console_callback_plug);


		int print(const wchar_t* text, int length);


		template <size_t buffer_size = 128>
		int printf(const wchar_t* format, ...);
	}

}