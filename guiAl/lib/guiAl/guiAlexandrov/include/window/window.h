#pragma once

// windows 10 style
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#include "gui_init.h"
#include "io/input.h"
#include "image/color.h"
#include "image/text.h"
#include "image/canvas.h"
#include "error_handler/errors.h"
#include "window/components.h"

#include <Windows.h>
#include <functional>
#include <utility>
#include <vector>
#include <string>


// ========================================= CALLBACK ARGUMENTS ========================================================

struct Args
{
	void* vals[2];
	std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback;

	void* operator [] (int i);
};


namespace gui
{
	struct Arguments
	{
		std::vector<std::pair<HWND, Args>> buffer;

		void add(HWND hwnd, void* pwindow, void* parg, std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback);

		Args get(HWND hwnd);

		void remove(HWND hwnd);
	};
	extern Arguments _arguments;




	// ========================================= WINDOW ========================================================

#define DEF_WINDOW	WS_OVERLAPPEDWINDOW | WS_VISIBLE
	typedef int WindowId;

	struct Window
	{
		static int _name_id;
		static std::vector<std::pair<WindowId, Window*>> windows;

		WindowId id = 0;
		HWND hwnd = 0;
		HDC hdc;
		Canvas canvas;
		int min_w = 0, min_h = 0;
		int max_w = 600, max_h = 800;

		Window();
		Window(
			const std::wstring& window_name,
			int width,
			int height,
			std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback =
				[](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args) { return DefWindowProc(hwnd, msg, wParam, lParam); },
			UINT style = DEF_WINDOW,
			HWND parent = NULL
		);
		
		bool init(
			const std::wstring& window_name,
			int width,
			int height,
			std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback = 
				[](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args) { return DefWindowProc(hwnd, msg, wParam, lParam); },
			UINT style = DEF_WINDOW,
			HWND parent = NULL
		);
		

		virtual ~Window();
		
		void close();
		
		void render_canvas(const PAINTSTRUCT& ps);
		void render_canvas();
		
		void redraw();

		int height();
		int width();
		
		bool valid();
		
#define MAX_WIN_SIZE -1
#define OLD_WIN_SIZE -2

		void set_min_max_size(int minw, int minh, int maxw = OLD_WIN_SIZE, int maxh = OLD_WIN_SIZE);

		HWND getHWND();

		// static elements
		static Window* get_window(int id);

		static bool is_running(WindowId id);
		static bool is_running(Window* window);

		static HWND getHWND(WindowId id);
		static void close(WindowId id);
		
		static void default_msg_proc();
		static void wait_msg_proc();
	};


	// Window layout
	WindowId create_window(const std::wstring& window_name,
		int width,
		int height,
		std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback = 
			[](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args) { return DefWindowProc(hwnd, msg, wParam, lParam); },
		UINT style = DEF_WINDOW,
		HWND parent = NULL);

}