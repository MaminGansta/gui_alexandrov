#pragma once

#include <string>
#include <vector>
#include <windows.h>

#pragma comment( lib, "comctl32.lib" )
#include <CommCtrl.h>
#include <windowsx.h>

#include "error_handler/errors.h"
#include "window/gui_init.h"

namespace gui
{
	// ====================================== WINDOW COMPONENTS ============================================

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
	extern Component_crt _components;


	// Compoent_id
	extern int _global_id;
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