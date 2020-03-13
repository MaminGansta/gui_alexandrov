
#define DEF_WINDOW (WS_OVERLAPPEDWINDOW | WS_VISIBLE)


// =========================================== CALLBACK ARGUMENTS =============================================================

struct Args
{
	void* vals[2];
	std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback;

	void* operator [] (int i) { return vals[i]; }
};

struct Arguments
{
	std::vector<std::pair<HWND, Args>> buffer;

	void add(HWND hwnd, void* pwindow, void* parg, std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback)
	{
		buffer.push_back(std::make_pair(hwnd, Args{ pwindow, parg, callback}));
	}

	Args get(HWND hwnd)
	{
		auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, Args> in) { return hwnd == in.first; });
		return it == buffer.end() ? Args{NULL,NULL} : it->second;
	}

	void remove(HWND hwnd)
	{
		auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, Args> in) { return hwnd == in.first; });
		if (it != buffer.end())
			buffer.erase(it);
	}
};
Arguments arguments;



// ================================== WINDOW COMPONENTS ====================================================


#define STATIC 0
#define DYNAMIC 1
#define RESIZABLE 2

struct Component
{
	int id;
	float x, y, width, height;
	int type;
	HWND handle;
	HWND parent;

	void resize()
	{
		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		UINT flags = SWP_NOZORDER;

		if (type == STATIC)
			flags = SWP_NOMOVE | SWP_NOSIZE;
		else if (type == DYNAMIC)
			flags = SWP_NOSIZE;

		SetWindowPos(handle, 0, x * nWidth, y * nHeight, width * nWidth, height * nHeight, flags);
	}
};


struct Component_crt
{
	std::unordered_map<HWND, std::vector<Component>> components;

	void add(HWND parent, Component* comp)
	{
		components[parent].push_back(*comp);
	}

	void remove(HWND parent)
	{
		components.erase(parent);
	}

	void update(HWND parent)
	{
		for (auto component : components[parent])
		{
			component.resize();
		}
	}
};
Component_crt components;



// ==============================	 HWND HANDLER ======================================
struct HWND_constainer
{
	int gen_id = 0;
	std::vector<std::pair<int, HWND>> handles;

	int add(HWND handle)
	{
		handles.push_back(std::make_pair(gen_id, handle));
		return gen_id++;
	}

	void remove(int id)
	{
		auto handle = std::find_if(handles.begin(), handles.end(), [id](std::pair<int, HWND> in) {return in.first == id; });
		arguments.remove(handle->second);
		DestroyWindow(handle->second);
		handles.erase(handle);
	}

	HWND operator [](int id)
	{
		auto res = std::find_if(handles.begin(), handles.end(), [id](std::pair<int, HWND> in) {return in.first == id; });

		if (res == handles.end())
			return NULL;

		return res->second;
	}

	int size() { return handles.size(); };
};

HWND_constainer handles;



// ========================================= WINDOW ========================================================
struct Window
{	
	int class_id;
	static int name_id;
	HDC hdc;
	Canvas canvas;

	Window() {}
	
	Window(
		const std::wstring& window_name,
		int width,
		int height,
		std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback,
		UINT style = DEF_WINDOW,
		HWND parent = NULL,
		void* arg_ptr = NULL,
		int id = 0
	)
	{
		init(window_name, width, height, style, parent, arg_ptr, callback , id);
	}

	void init(
		const std::wstring& window_name,
		int width,
		int height,
		UINT style,
		HWND parent,
		void* arg_ptr,
		std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback,
		int id = 0
	)
	{
		wchar_t class_name[16];
		swprintf_s(class_name, L"class_%d", name_id++);
		std::wstring name(class_name);

		WNDCLASSEX wc;
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = name.c_str();
		//  wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);


		// global callback function
		wc.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT 
		{
			Args args = arguments.get(hwnd);
			Window* window = (Window*)args[0];
			if (window == NULL) return DefWindowProc(hwnd, msg, wParam, lParam);

			if (msg == WM_SIZE)
			{
				components.update(hwnd);
				window->canvas.resize(hwnd);
			}
			
			int res = args.callback(hwnd, msg, wParam, lParam, args);

			return res == -1 ? DefWindowProc(hwnd, msg, wParam, lParam) : res;
		};


		if (!RegisterClassEx(&wc))
		{
			MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
			assert(false);
		}

		HWND handle = CreateWindow(wc.lpszClassName, window_name.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, parent, (HMENU)id, (HINSTANCE)hInst, NULL);

		arguments.add(handle, this, arg_ptr, callback);

		SendMessage(handle, WM_CREATE, 0, 0);
		SendMessage(handle, WM_SIZE, 0, 0);

		class_id = handles.add(handle);
		hdc = GetDC(handle);
	}

	virtual ~Window() { handles.remove(class_id); }

	void render_canvas()
	{
		StretchDIBits(hdc, 0, 0, canvas.width, canvas.height, 0, 0, canvas.width, canvas.height, canvas.memory, &canvas.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}

	HWND getHWND() { return handles[class_id]; }

	static void default_msg_proc() {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	static void wait_msg_proc() {
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
};

int Window::name_id = 0;



// ==============================  Window Components =================================================

// ================== Button =====================
struct Button : Component
{
	Button() = default;
	Button(
		HWND parent,
		const std::wstring& button_name,
		int id,
		float x,
		float y,
		float width,
		float height,
		UINT type = DYNAMIC,
		UINT style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON
	)
	{
		init(parent, button_name, id, x, y, width, height, type, style);
	}

	void init(
		HWND parent,
		const std::wstring& button_name,
		int id,
		float x,
		float y,
		float width = 0.1f,
		float height = 0.1f,
		UINT type = DYNAMIC,
		UINT style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON
		)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			button_name.c_str(), // Button text 
			style,     // Styles 
			x * nWidth,         // x position 
			y * nHeight,         // y position 
			width * nWidth,     // Button width
			height * nHeight,    // Button height
			parent,    // Parent window
			(HMENU)id, // menu.
			hInst,
			NULL);      // Pointer not needed.

		components.add(parent, this);
	}

	~Button() { DestroyWindow(handle); };
};

// =============== Radio Button ======================
struct RadioButton : Component
{
	RadioButton() = default;
	RadioButton(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC)
	{
		init(parent, text, id, x, y, width, height, type);
	}

	void init(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(L"Button", text.c_str(),
			BS_AUTORADIOBUTTON | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE ,
			x * nWidth, y * nHeight, width * nWidth, height * nHeight, parent, (HMENU)id, hInst, NULL);
	
		components.add(parent, this);
	}

	~RadioButton() { DestroyWindow(handle); }
};


// ================ Check Button =================
struct CheckBox : Component
{
	CheckBox() = default;
	CheckBox(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC)
	{
		init(parent, text, id, x, y, width, height, type);
	}

	void init(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(L"Button", text.c_str(),
			BS_AUTOCHECKBOX | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE ,
			x* nWidth, y* nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);
	
		components.add(parent, this);
	}

	~CheckBox() { DestroyWindow(handle); }
};

// =================== ComboBox ========================
struct ComboBox : Component
{
	ComboBox() = default;
	ComboBox(HWND parent, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC)
	{
		init(parent, id, x, y, width, height, type);
	}

	void init(HWND parent, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(L"ComboBox", TEXT("combo"),
			CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
			x* nWidth, y* nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);

		components.add(parent, this);

	}

	void add(const std::wstring& element)
	{
		// Add string to combobox.
		SendMessage(handle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		// Send the CB_SETCURSEL message to display an initial item 
		//  in the selection field  
		SendMessage(handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	}

	void add(std::vector<std::wstring> elements)
	{
		// Add strings to combobox.
		for (auto& element : elements)
			SendMessage(handle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		SendMessage(handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	}

	void remove(int idx)
	{
		SendMessage(handle, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}

	void remove(const std::wstring& name)
	{
		int idx = SendMessage(handle, CB_FINDSTRING, (WPARAM)0, (LPARAM)0);
		SendMessage(handle, CB_DELETESTRING, (WPARAM)idx, (LPARAM)0);
	}

	void clear()
	{
		int count = SendMessage(handle, CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
		for (int i = 0; i < count; i++)
			SendMessage(handle, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}

	~ComboBox() { DestroyWindow(handle); }

	// -----------------  STATIC ELEMENTS  --------------------

	static std::wstring choosed(LPARAM lParam)
	{
		int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		TCHAR  ListItem[256];
		(TCHAR)SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)ListItem);
		return std::wstring(ListItem);
	}

	static void add(HWND handle, const std::wstring& element)
	{
		// Add string to combobox.
		SendMessage(handle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		// Send the CB_SETCURSEL message to display an initial item 
		//  in the selection field  
		SendMessage(handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	}

	static void add(LPARAM  handle, std::vector<std::wstring> elements)
	{
		// Add strings to combobox.
		for (auto& element : elements)
			SendMessage((HWND)handle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		SendMessage((HWND)handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	}

	static void remove(LPARAM  handle, int idx)
	{
		SendMessage((HWND)handle, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}

	static void remove(LPARAM  handle, const std::wstring& name)
	{
		int idx = SendMessage((HWND)handle, CB_FINDSTRING, (WPARAM)0, (LPARAM)name.c_str());
		SendMessage((HWND)handle, CB_DELETESTRING, (WPARAM)idx, (LPARAM)0);
	}

	static void clear(LPARAM  handle)
	{
		int count = SendMessage((HWND)handle, CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
		for (int i = 0; i < count; i++)
			SendMessage((HWND)handle, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}
};


// ================ Label =================
struct Label : Component
{
	Label() = default;
	Label(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC)
	{
		init(parent, text, id, x, y, width, height, type);
	}

	void init(HWND parent, const std::wstring& text, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(L"static", L"label", WS_CHILD | WS_VISIBLE  | SS_LEFT,
		x* nWidth, y* nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);

		components.add(parent, this);
		SetWindowText(handle, text.c_str());
	}

	void set_text(const std::wstring& text)
	{
		SetWindowText(handle, text.c_str());
	}

	~Label() { DestroyWindow(handle); }
};

// ========================= Text ================================
struct Text : Component
{
	TCHAR* text = NULL;
	
	Text() = default;
	Text(HWND parent, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC)
	{
		init(parent, id, x, y, width, height, type);
	}

	void init(HWND parent, int id, float x, float y, float width = 0.1f, float height = 0.1f, UINT type = DYNAMIC)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->parent = parent;
		this->type = type;

		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;

		handle = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 
		x* nWidth, y* nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, hInst, NULL);

		components.add(parent, this);
	}

	TCHAR* getText()
	{
		delete[] text;
		int nLength = GetWindowTextLength(handle);
		if (nLength > 0)
		{
			text = new TCHAR[nLength + 1];
			GetWindowText(handle, text, nLength + 1);
		}
		return text;
	}

	void SetText(TCHAR* text)
	{
		SetWindowText(handle, text);
	}
		
	~Text() { DestroyWindow(handle); delete[] text; }
};

