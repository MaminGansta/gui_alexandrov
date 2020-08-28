
#include "window/components.h"


namespace gui
{

	// ================== Button =====================

	Button::Button(const Button& other)
	{
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
	}

	Button& Button::operator= (const Button& other)
	{
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
		return *this;
	}

	Button::Button(
		HWND parent,
		const std::wstring& text,
		float x,
		float y,
		float width,
		float height,
		UINT type,
		UINT style
	)
	{
		init(parent, text, x, y, width, height, type, style);
	}

	bool Button::init(
		HWND parent,
		const std::wstring& text,
		float x,
		float y,
		float width,
		float height,
		UINT type,
		UINT style
	)
	{
		this->parent = parent;
		this->text = text;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			text.c_str(), // Button text 
			style,     // Styles 
			x * nWidth,         // x position 
			(1.0f - y - height) * nHeight,         // y position 
			width * nWidth,     // Button width
			height * nHeight,    // Button height
			parent,    // Parent window
			(HMENU)id, // menu.
			_hInst,
			NULL);      // Pointer not needed.

		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		_components.add(parent, comp);
		return true;
	}


	// =============== Radio Button ======================

	RadioButton::RadioButton() = default;
	RadioButton::RadioButton(const RadioButton & other)
	{
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
	}

	RadioButton& RadioButton::operator= (const RadioButton & other)
	{
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
		return *this;
	}

	RadioButton::RadioButton(HWND parent, const std::wstring & text, float x, float y, float width, float height, UINT type, UINT style)
	{
		init(parent, text, x, y, width, height, type, style);
	}

	bool RadioButton::RadioButton::init(HWND parent, const std::wstring & text, float x, float y, float width, float height, UINT type, UINT style)
	{
		this->parent = parent;
		this->text = text;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(L"Button", text.c_str(), style,
			x * nWidth, (1.0f - y - height) * nHeight, width * nWidth, height * nHeight, parent, (HMENU)id, _hInst, NULL);

		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		_components.add(parent, comp);
		return true;
	}

	int RadioButton::chosed()
	{
		return SendMessage(hwnd, BM_GETCHECK, 0, 0);
	}



	// ================ Check Button =================

	CheckBox::CheckBox() = default;
	CheckBox::CheckBox(const CheckBox & other)
	{
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
	}

	CheckBox& CheckBox::operator= (const CheckBox & other)
	{
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
		return *this;
	}

	CheckBox::CheckBox(HWND parent, const std::wstring & text, float x, float y, float width, float height, UINT type, UINT style)
	{
		init(parent, text, x, y, width, height, type, style);
	}

	bool CheckBox::init(HWND parent, const std::wstring & text, float x, float y, float width, float height, UINT type, UINT style)
	{
		this->parent = parent;
		this->text = text;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(L"Button", text.c_str(), style,
			x * nWidth, (1.0f - y - height) * nHeight, width * nWidth, height * nHeight, parent, (HMENU)id, _hInst, NULL);

		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		_components.add(parent, comp);
		return true;
	}

	bool CheckBox::checked()
	{
		return SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
	}


	// =================== ComboBox ========================

	ComboBox::ComboBox() = default;
	ComboBox::ComboBox(const ComboBox & other)
	{
		Component* comp = other.get();
		init(comp->parent, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
	}

	ComboBox& ComboBox::operator= (const ComboBox & other)
	{
		Component* comp = other.get();
		init(comp->parent, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
		return *this;
	}

	ComboBox::ComboBox(HWND parent, float x, float y, float width, float height, UINT type, UINT style)
	{
		init(parent, x, y, width, height, type, style);
	}

	bool ComboBox::init(HWND parent, float x, float y, float width, float height, UINT type, UINT style)
	{
		this->parent = parent;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(L"ComboBox", L"combo", style,
			x * nWidth, (1.0f - y - height) * nHeight, width * nWidth, height * nHeight, parent, (HMENU)id, _hInst, NULL);

		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		_components.add(parent, comp);
		return true;
	}

	void ComboBox::add(const std::wstring & element)
	{
		// Add string to combobox.
		SendMessage(hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		// Send the CB_SETCURSEL message to display an initial item 
		//  in the selection field  
		SendMessage(hwnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	}

	void ComboBox::add(const std::vector<std::wstring> & elements)
	{
		// Add strings to combobox.
		for (auto& element : elements)
			SendMessage(hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		SendMessage(hwnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	}

	void ComboBox::set_selected(int i)
	{
		ComboBox_SetCurSel(hwnd, i);
	}

	UINT ComboBox::choosed_index()
	{
		return SendMessage(hwnd, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	}

	std::wstring ComboBox::choosed()
	{
		int ItemIndex = SendMessage(hwnd, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		TCHAR  ListItem[256];
		(TCHAR)SendMessage(hwnd, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)ListItem);
		return std::wstring(ListItem);
	}

	void ComboBox::remove(int idx)
	{
		SendMessage(hwnd, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}

	void ComboBox::remove(const std::wstring & name)
	{
		int idx = SendMessage(hwnd, CB_FINDSTRING, (WPARAM)0, (LPARAM)0);
		SendMessage(hwnd, CB_DELETESTRING, (WPARAM)idx, (LPARAM)0);
	}

	void ComboBox::clear()
	{
		int count = SendMessage(hwnd, CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
		for (int i = 0; i < count; i++)
			SendMessage(hwnd, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);

		ComboBox_SetText(hwnd, L"");
	}


	// ================ Label =================

	Label::Label() = default;
	Label::Label(const Label & other)
	{
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
	}

	Label& Label::operator= (const Label & other)
	{
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
		return *this;
	}

	Label::Label(HWND parent, const std::wstring & text, float x, float y, float width, float height, UINT type, UINT style)
	{
		init(parent, text, x, y, width, height, type, style);
	}

	bool Label::init(HWND parent, const std::wstring & text, float x, float y, float width, float height, UINT type, UINT style)
	{
		this->parent = parent;
		this->text = text;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(L"static", L"label", style,
			x * nWidth, (1.0f - y - height) * nHeight, width * nWidth, height * nHeight, parent, (HMENU)id, _hInst, NULL);

		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		_components.add(parent, comp);
		SetWindowText(hwnd, text.c_str());
		return true;
	}

	void Label::set_text(const std::wstring & text)
	{
		SetWindowText(hwnd, text.c_str());
	}


	// ========================= Text ================================

	Text::Text() = default;
	Text::Text(const Text & other)
	{
		Component* comp = other.get();
		init(comp->parent, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
	}

	Text& Text::operator= (const Text & other)
	{
		Component* comp = other.get();
		init(comp->parent, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
		return *this;
	}

	Text::Text(HWND parent, float x, float y, float width, float height, UINT type, UINT style)
	{
		init(parent, x, y, width, height, type, style);
	}

	bool Text::init(HWND parent, float x, float y, float width, float height, UINT type, UINT style)
	{
		this->parent = parent;
		text = (TCHAR*)::operator new(cap * sizeof(TCHAR));

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(L"edit", L"", style,
			x * nWidth, (1.0f - y - height) * nHeight, width * nWidth, height * nHeight, parent, (HMENU)id, _hInst, NULL);

		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		_components.add(parent, comp);
		return true;
	}

	TCHAR* Text::get_text()
	{
		int nLength = GetWindowTextLength(hwnd);
		if (nLength > 0)
		{
			if (nLength > cap)
			{
				cap = nLength + 1;
				::operator delete(text);
				text = (TCHAR*)::operator new(cap * sizeof(TCHAR));
			}
			GetWindowText(hwnd, text, nLength + 1);
			return text;
		}
		return NULL;
	}

	void Text::set_text(const TCHAR * text)
	{
		SetWindowText(hwnd, text);
	}

	void Text::clear()
	{
		*text = '\0';
		SetWindowText(hwnd, NULL);
	}

	Text::~Text() { ::operator delete(text); }


	// ========================= Table ================================ Not a table actually and very slow, because rows*cols windows's hwndls are used

	Table::Table() = default;
	Table::Table(HWND paretn, int max_row, int max_col, float x, float y, float width, float height, int size_type, int type)
	{
		init(paretn, max_row, max_col, x, y, width, height, size_type, type);
	}

	bool Table::init(HWND parent, int max_row, int max_col, float x, float y, float width, float height, int size_type, int type)
	{
		this->cap_row = max_row;
		this->cap_col = max_col;
		this->size_type = size_type;
		this->parent = parent;

		UINT lStyle = WS_CHILD | WS_VISIBLE | SS_CENTER;
		UINT tStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER;

		col_labels.resize(max_col, Label(parent, L"", 0, 0, 0, 0, type, lStyle));
		row_labels.resize(max_row, Label(parent, L"", 0, 0, 0, 0, type, lStyle));
		table.resize(max_row * max_col, Text(parent, 0, 0, 0, 0, type, tStyle));

		for (int i = 0; i < max_col; i++)
			col_labels[i].hide();

		for (int i = 0; i < max_row; i++)
			row_labels[i].hide();

		for (int i = 0; i < max_col * max_row; i++)
			table[i].hide();

		// just indicator of this class
		this->hwnd = hwnd = CreateWindow(L"BUTTON", L"", 0, 0, 0, 0, 0, parent, NULL, _hInst, NULL);

		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}

		Component comp(id, x, y, width, height, type, STATIC, hwnd, parent);
		_components.add(parent, comp);
		return true;
	}

	void Table::create(const std::vector<std::wstring> & text_rows, const std::vector<std::wstring> & text_cols)
	{
		if (text_rows.size() > cap_row || text_cols.size() > cap_col) return;
		if (text_rows.size() == 0 && text_cols.size() == 0) return;

		Component* comp = get();

		// hide old elements
		for (int i = 0; i < cols; i++)
			col_labels[i].hide();

		for (int i = 0; i < rows; i++)
			row_labels[i].hide();

		for (int i = 0; i < cols * rows; i++)
			table[i].hide();

		// rename and move elements for new pos
		rows = text_rows.size();
		cols = text_cols.size();

		for (int i = 0; i < rows; i++)
			row_labels[i].set_text(text_rows[i]);

		for (int i = 0; i < cols; i++)
			col_labels[i].set_text(text_cols[i]);

		float cell_w, cell_h;
		switch (size_type)
		{
		case TOTAL_SIZE:
		{
			cell_w = comp->width / (1 + cols);
			cell_h = comp->height / (1 + rows);
		}break;
		case CELL_SIZE:
		{
			cell_h = comp->height;
			cell_w = comp->width;
		}break;
		}

		for (int i = 0; i < cols; i++)
		{
			col_labels[i].move(comp->x + cell_w * i + cell_w, comp->y);
			col_labels[i].resize(cell_w, cell_h);
			col_labels[i].show();
		}
		for (int i = 0; i < rows; i++)
		{
			row_labels[i].move(comp->x, comp->y + cell_h * i + cell_h);
			row_labels[i].resize(cell_w, cell_h);
			row_labels[i].show();
		}

		rows = rows ? rows : 1;
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				table[i * cols + j].move(comp->x + cell_w * j + cell_w, comp->y + cell_h * i + cell_h);
				table[i * cols + j].resize(cell_w, cell_h);
				table[i * cols + j].show();
			}
		}
	}

	Text* Table::operator[] (int i)
	{
		return table.data() + i * cols;
	}

	std::vector<TCHAR*> Table::get_data()
	{
		std::vector<TCHAR*> data;

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				data.push_back(table[i * cols + j].get_text());

		return data;
	}

	void Table::clear()
	{
		for (int i = 0; i < cap_row; i++)
			for (int j = 0; j < cap_col; j++)
				table[i * cap_col + j].clear();
	}


	// ==================== ListView =========================

	ListView::ListView() = default;
	ListView::ListView(HWND parent, float x, float y, float width, float height, UINT type, UINT style)
	{
		init(parent, x, y, width, height, type, style);
	}

	bool ListView::init(HWND parent, float x, float y, float width, float height, UINT type, UINT style)
	{

		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);


		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		hwnd = CreateWindow(WC_LISTVIEW, L"", style,
			x * nWidth, (1.0f - y - height) * nHeight, width * nWidth, height * nHeight,
			parent, (HMENU)id, _hInst, NULL);

		if (!hwnd)
		{
			error_list.push_back(7);
			return false;
		}

		ListView_SetExtendedListViewStyleEx(hwnd, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		this->parent = parent;

		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		_components.add(parent, comp);
		return true;
	}

	void ListView::add_columns(std::vector<std::wstring> columns)
	{
		Component* comp = get();

		this->columns = columns.size();
		LVCOLUMN lvc;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;

		int colum_size = nWidth * comp->width / columns.size();

		// Initialize the LVCOLUMN structure.
		// The mask specifies that the format, width, text,
		// and subitem members of the structure are valid.
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVIF_COLFMT;

		// Add the columns.
		for (int iCol = 0; iCol < columns.size(); iCol++)
		{
			lvc.iSubItem = iCol;
			lvc.pszText = (LPWSTR)columns[iCol].c_str();
			lvc.cx = colum_size;               // Width of column in pixels.

			if (iCol < 2)
				lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
			else
				lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

			// Insert the columns into the list view.
			//if (ListView_InsertColumn(hwnd, iCol, &lvc) == -1)
			//	doutput("listview insert problem");
		}
	}

	void ListView::add_row(std::vector<std::wstring> row)
	{
		int textMaxLen = 10;

		int iLastIndex = ListView_GetItemCount(hwnd);

		LVITEM lvi;
		lvi.mask = LVIF_TEXT;
		lvi.cchTextMax = textMaxLen;
		lvi.iItem = iLastIndex;
		lvi.pszText = (LPWSTR)row[0].c_str();
		lvi.iSubItem = 0;

		if (ListView_InsertItem(hwnd, &lvi) != -1)
			for (int i = 1; i < row.size(); i++)
				ListView_SetItemText(hwnd, iLastIndex, i, (LPWSTR)row[i].c_str());

		// align the table
		ListView_SetColumnWidth(hwnd, 1, LVSCW_AUTOSIZE);
	}

	void ListView::add_rows(std::vector<std::vector<std::wstring>> rows)
	{
		int textMaxLen = 10;

		for (int i = 0; i < rows.size(); i++)
		{
			int iLastIndex = ListView_GetItemCount(hwnd);

			LVITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.cchTextMax = textMaxLen;
			lvi.iItem = iLastIndex;
			lvi.pszText = (LPWSTR)rows[i][0].c_str();
			lvi.iSubItem = 0;

			if (ListView_InsertItem(hwnd, &lvi) != -1)
				for (int j = 1; j < rows[i].size(); j++)
					ListView_SetItemText(hwnd, iLastIndex, j, (LPWSTR)rows[i][j].c_str());

			// align the table
			ListView_SetColumnWidth(hwnd, 1, LVSCW_AUTOSIZE);
		}
	}

	void ListView::space(int i)
	{
		wchar_t buffer[] = L"";

		for (int j = 1; j < i; j++)
		{
			int iLastIndex = ListView_GetItemCount(hwnd);

			LVITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.cchTextMax = 10;
			lvi.iItem = iLastIndex;
			lvi.pszText = (LPWSTR)buffer;
			lvi.iSubItem = 0;

			if (ListView_InsertItem(hwnd, &lvi) != -1)
				ListView_SetItemText(hwnd, iLastIndex, j, (LPWSTR)buffer);
		}

	}

	void ListView::clear()
	{
		ListView_DeleteAllItems(hwnd);
	}

	void ListView::remove_row(int i)
	{
		ListView_DeleteItem(hwnd, i);
	}

	int ListView::rows()
	{
		return ListView_GetItemCount(hwnd);
	}

}