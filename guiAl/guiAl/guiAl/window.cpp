
#define DEF_STYLE (WS_OVERLAPPEDWINDOW | WS_VISIBLE)


struct args 
{
	void* vals[2];
	void* operator [] (int i) { return vals[i]; }
};

struct Arguments
{
	std::vector<std::pair<HWND, args>> buffer;

	void add(HWND hwnd, void* pwindow, void* parg)
	{
		buffer.push_back(std::make_pair(hwnd, args{ pwindow, parg }));
	}

	args get(HWND hwnd)
	{
		auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, args> in) { return hwnd == in.first; });
		return it == buffer.end() ? args{NULL,NULL} : it->second;
	}

	void remove(HWND hwnd)
	{
		auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, args> in) { return hwnd == in.first; });
		if (it != buffer.end())
			buffer.erase(it);
	}
};
Arguments arguments;


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

	void proc_msg()
	{
		for (auto& handle : handles)
		{
			MSG msg;
			while (PeekMessage(&msg, handle.second, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	int size() { return handles.size(); };
};


struct Window
{
	static HWND_constainer handles;
	
	static int class_id;
	HDC hdc;
	Canvas canvas;

	Window() {}
	
	Window(
		std::wstring window_name,
		int width,
		int height,
		UINT style,
		HWND parent,
		void* arg_ptr,
		LRESULT(CALLBACK* callback)(HWND, UINT, WPARAM, LPARAM),
		int id = 0
	)
	{
		create_window(window_name, width, height, style, parent, arg_ptr, callback , id);
	}

	void create_window(
		std::wstring window_name,
		int width,
		int height,
		UINT style,
		HWND parent,
		void* arg_ptr,
		LRESULT(CALLBACK* callback)(HWND, UINT, WPARAM, LPARAM),
		int id = 0
	)
	{
		wchar_t class_name[16];
		swprintf_s(class_name, L"image_%d", class_id++);
		std::wstring name(class_name);

		WNDCLASSEX wc;
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = callback;
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

		if (!RegisterClassEx(&wc))
		{
			MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
			assert(false);
		}

		HWND handle = CreateWindow(wc.lpszClassName, window_name.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, parent, (HMENU)id, (HINSTANCE)hInst, NULL);

		arguments.add(handle, this, arg_ptr);

		SendMessage(handle, WM_CREATE, 0, 0);
		SendMessage(handle, WM_SIZE, 0, 0);

		class_id = handles.add(handle);
		hdc = GetDC(handle);
	}

	~Window() { handles.remove(class_id); }

	void render_canvas()
	{
		StretchDIBits(hdc, 0, 0, canvas.width, canvas.height, 0, 0, canvas.width, canvas.height, canvas.memory, &canvas.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}

	HWND getHWND() { return handles[class_id]; }

	static void default_msg_proc() { handles.proc_msg(); }
};

int Window::class_id = 0;
HWND_constainer Window::handles = HWND_constainer();
