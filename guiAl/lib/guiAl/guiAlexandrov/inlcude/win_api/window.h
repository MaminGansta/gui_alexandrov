#pragma once

// windows 10 look
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#include "gui_init.h"
#include "../io/input.h"
#include "../image/color.h"
#include "../image/text.h"
#include "../image/canvas.h"
#include "../error_handler/errors.h"

#pragma comment( lib, "comctl32.lib" )
#include <CommCtrl.h>
#include <windowsx.h>

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


struct Arguments
{
	std::vector<std::pair<HWND, Args>> buffer;

	void add(HWND hwnd, void* pwindow, void* parg, std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback);

	Args get(HWND hwnd);

	void remove(HWND hwnd);
};
extern Arguments arguments;



namespace gui
{

	// =========================================== WINDOW COMPONENTS ====================================================

#define STATIC 0
#define DYNAMIC 1
#define RESIZABLE 2

	// Component
	struct Component
	{
		int  id;
		float x, y, width, height;
		UINT type;
		UINT style;
		HWND hwnd;
		HWND parent;

		Component(int id, float x, float y, float width, float height,
			UINT type, UINT style, HWND hwnd, HWND parent);
		

		virtual ~Component();

		void resize(RECT& rect);

		void resize(float width, float height);

		void move(float x, float y);
	

		void hide();
		void show();
	};


	// Component_crt
	struct Component_crt
	{
		std::vector<std::pair<HWND, std::vector<Component>>> components;

		void add(HWND parent, Component& comp);
		
		void remove(HWND parent);
	
		void resize(HWND parent);

		void redraw(HWND parent);
	
		auto& operator[] (HWND parent);
		

	};
	extern Component_crt components;


	// Compoent_id
	extern int global_id;
	struct Component_id
	{
		int id;
		HWND parent;
		HWND hwnd;

		Component_id();

		Component* get() const;
		
		void resize(float width, float height);
		
		void move(float x, float y);

		void hide();
		void show();
	};



	// ========================================= WINDOW ========================================================

#define DEF_WINDOW	WS_OVERLAPPEDWINDOW | WS_VISIBLE
	typedef int WindowId;

	struct Window
	{
		static int name_id;
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
		int widht();
		
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


	// ==============================  Window Components =================================================

	// ================== Button =====================
#define DEF_BUTTON WS_TABSTOP | WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE

	struct Button : Component_id
	{
		std::wstring text;

		Button() = default;
		Button(const Button& other);
		Button& operator= (const Button& other);

		Button(
			HWND parent,
			const std::wstring& text,
			float x,
			float y,
			float width,
			float height,
			UINT type = RESIZABLE,
			UINT style = DEF_BUTTON
		);

		bool init(
			HWND parent,
			const std::wstring& text,
			float x,
			float y,
			float width = 0.1f,
			float height = 0.1f,
			UINT type = RESIZABLE,
			UINT style = DEF_BUTTON
		);
	};

	// =============== Radio Button ======================
#define DEF_RADIO	BS_AUTORADIOBUTTON | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE

	struct RadioButton : Component_id
	{
		std::wstring text;

		RadioButton();
		RadioButton(const RadioButton& other);
		RadioButton& operator= (const RadioButton& other);

		RadioButton(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_RADIO);

		bool init(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_RADIO);
		
		int chosed();
	};


	// ================ Check Button =================
#define DEF_CHECK	BS_AUTOCHECKBOX | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE

	struct CheckBox : Component_id
	{
		std::wstring text;

		CheckBox();
		CheckBox(const CheckBox& other);
		CheckBox& operator= (const CheckBox& other);

		CheckBox(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_CHECK);

		bool init(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_CHECK);
		
		bool checked();
	};







	// =================== ComboBox ========================
#define DEF_COMBO	CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE

	struct ComboBox : Component_id
	{
		ComboBox();
		ComboBox(const ComboBox& other);
		ComboBox& operator= (const ComboBox& other);

		ComboBox(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_COMBO);

		bool init(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_COMBO);

		void add(const std::wstring& element);

		void add(const std::vector<std::wstring>& elements);

		void set_selected(int i);

		UINT choosed_index();

		std::wstring choosed();

		void remove(int idx);

		void remove(const std::wstring& name);

		void clear();

	};


	// ================ Label =================
#define DEF_LABEL (WS_CHILD | WS_VISIBLE  | SS_LEFT)

	struct Label : Component_id
	{
		std::wstring text;

		Label();
		Label(const Label& other);
		Label& operator= (const Label& other);

		Label(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_LABEL);

		bool init(HWND parent, const std::wstring& text, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_LABEL);

		void set_text(const std::wstring& text);
	};

	// ========================= Text ================================
#define DEF_TEXT WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL

	struct Text : Component_id
	{
		int cap = 10;
		TCHAR* text = NULL;

		Text();
		Text(const Text& other);
		Text& operator= (const Text& other);

		Text(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_TEXT);

		bool init(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_TEXT);

		TCHAR* get_text();

		void set_text(const TCHAR* text);

		void clear();

		~Text();
	};


	// ========================= Table ================================ Not a table actually and very slow, because rows*cols windows's hwndls are used
#define TOTAL_SIZE 0
#define CELL_SIZE 1

	struct Table : Component_id
	{
		int cap_col = 0, cap_row = 0;
		int cols = 0, rows = 0;
		int size_type;
		std::vector<Label> col_labels;
		std::vector<Label> row_labels;
		std::vector<Text> table;

		Table();
		Table(HWND paretn, int max_row, int max_col, float x, float y, float width, float height, int size_type = TOTAL_SIZE, int type = RESIZABLE);

		bool init(HWND parent, int max_row, int max_col, float x, float y, float width, float height, int size_type = TOTAL_SIZE, int type = RESIZABLE);

		void create(const std::vector<std::wstring>& text_rows, const std::vector<std::wstring>& text_cols);

		Text* operator[] (int i);

		std::vector<TCHAR*> get_data();

		void clear();
	};


	// ==================== ListView =========================
#define DEF_LISTVIEW	WS_CHILD | LVS_REPORT | WS_VISIBLE

	struct ListView : Component_id
	{
		int columns = 0;

		ListView();
		ListView(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_LISTVIEW);

		bool init(HWND parent, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = RESIZABLE, UINT style = DEF_LISTVIEW);

		void add_columns(std::vector<std::wstring> columns);

		void add_row(std::vector<std::wstring> row);

		void add_rows(std::vector<std::vector<std::wstring>> rows);

		void space(int i = 0);

		void clear();

		void remove_row(int i);

		int rows();
	};

}