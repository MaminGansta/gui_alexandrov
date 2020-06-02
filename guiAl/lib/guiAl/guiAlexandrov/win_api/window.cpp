
#include "win_api/window.h"


// =========================================== CALLBACK ARGUMENTS ========================================================

// Args
void* Args::operator [] (int i) { return vals[i]; }


//Arguments
void Arguments::add(HWND hwnd, void* pwindow, void* parg, std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback)
{
	buffer.push_back(std::make_pair(hwnd, Args{ pwindow, parg, callback}));
}

Args Arguments::get(HWND hwnd)
{
	auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, Args> in) { return hwnd == in.first; });
	return it == buffer.end() ? Args{NULL,NULL} : it->second;
}

void Arguments::remove(HWND hwnd)
{
	auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, Args> in) { return hwnd == in.first; });
	if (it != buffer.end())
		buffer.erase(it);
}

// global
Arguments arguments;



namespace gui
{
	// =========================================== WINDOW COMPONENTS LAYOUT ====================================================
	
	// Component

	Component::Component(int id, float x, float y, float width, float height,
		      UINT type, UINT style, HWND hwnd, HWND parent)
		: 
		id(id), x(x), y(y), width(width), height(height),
		type(type), style(style), hwnd(hwnd), parent(parent)
	{
		set_font(hwnd, DEF_FONT);
	}
	
	Component::~Component() {};
	
	void Component::resize(RECT& rect)
	{
		if (type == STATIC) return;
	
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;
	
		UINT flags = SWP_NOZORDER;
	
		if (type == DYNAMIC)
			flags = SWP_NOSIZE;
	
		SetWindowPos(hwnd, 0, x * nWidth, y * nHeight, width * nWidth + 1, height * nHeight + 1, flags);
	}
	
	void Component::resize(float width, float height)
	{
		this->height = height;
		this->width = width;
	
		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;
		
		SetWindowPos(hwnd, 0, 0, 0, width * nWidth + 1, height * nHeight + 1, SWP_NOZORDER | SWP_NOMOVE);
	}
	
	void Component::move(float x, float y)
	{
		this->x = x;
		this->y = y;
	
		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;
	
		SetWindowPos(hwnd, 0, x * nWidth, y * nHeight, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	
	void Component::hide() { ShowWindow(hwnd, SW_HIDE); }
	void Component::show() { ShowWindow(hwnd, SW_SHOWNOACTIVATE); }
	
	

	// Component crt

	void Component_crt::add(HWND parent, Component& comp)
	{
		auto pointer = std::find_if(components.begin(), components.end(), 
			[&parent](const std::pair<HWND, std::vector<Component>>& elem) {return elem.first == parent;});
		
		// if no such hwnd in storage
		if (pointer == components.end())
		{
			components.push_back(std::make_pair(parent, std::vector<Component>()));
			components.back().second.push_back(comp);
			return;
		}
	
		// add component to the parent
		pointer->second.push_back(comp);
	}
	
	void Component_crt::remove(HWND parent)
	{
		auto pointer = std::find_if(components.begin(), components.end(), 
			[&parent](const std::pair<HWND, std::vector<Component>>& elem) {return elem.first == parent;});
		if (pointer == components.end()) return;
	
		// release components of this parent
		for (auto& comp : pointer->second)
		  DestroyWindow(comp.hwnd);
	
		components.erase(pointer);
	}
	
	void Component_crt::resize(HWND parent)
	{
		auto pointer = std::find_if(components.begin(), components.end(), 
			[&parent](const std::pair<HWND, std::vector<Component>>& elem) {return elem.first == parent;});
		if (pointer == components.end()) return;
		
		RECT rect;
		GetClientRect(parent, &rect);
	
		for (auto& component : pointer->second)
			component.resize(rect);
	}
	
	void Component_crt::redraw(HWND parent)
	{
		auto pointer = std::find_if(components.begin(), components.end(), 
			[&parent](const std::pair<HWND, std::vector<Component>>& elem) {return elem.first == parent; });
		if (pointer == components.end()) return;
		
		for (auto& component : pointer->second)
			RedrawWindow(component.hwnd, 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
	}
	
	auto& Component_crt::operator[] (HWND parent)
	{
		auto pointer = std::find_if(components.begin(), components.end(), 
			[&parent](const std::pair<HWND, std::vector<Component>>& elem) {return elem.first == parent; });
	
		if (pointer == components.end())
		{
			components.push_back(std::make_pair(parent, std::vector<Component>()));
			return components.back().second;
		}
	
		return pointer->second;
	}
	
	// global
	Component_crt components;
	

	// Component_id

	// global
	int global_id = 0;
	
	Component_id::Component_id() : id(global_id++) {}
	
	Component* Component_id::get() const
	{
		// this is how c++ work
		int comp_id = id;
		auto& comps = components[parent];
		auto iterator = find_if(comps.begin(), comps.end(), [comp_id](const Component& comp) { return comp.id == comp_id; });
		return &(*iterator);
	}
	
	void Component_id::resize(float width, float height)
	{
		Component* comp = get();
	
		comp->height = height;
		comp->width = width;
	
		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;
	
		SetWindowPos(hwnd, 0, 0, 0, width * nWidth + 1, height * nHeight + 1, SWP_NOZORDER | SWP_NOMOVE);
	}
	
	void Component_id::move(float x, float y)
	{
		Component* comp = get();
	
		comp->x = x;
		comp->y = y;
	
		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;
	
		SetWindowPos(hwnd, 0, x * nWidth, y * nHeight, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	
	void Component_id::hide() { ShowWindow(hwnd, SW_HIDE); }
	void Component_id::show() { ShowWindow(hwnd, SW_SHOWNOACTIVATE); }
	

	
	
	// ========================================= WINDOW ========================================================
	
	Window::Window() = default;
	Window::Window(
		const std::wstring& window_name,
		int width,
		int height,
		std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback,
		UINT style,
		HWND parent
	)
	{
		init(window_name, width, height, callback, style, parent);
	}
	
	bool Window::init(
		const std::wstring& window_name,
		int width,
		int height,
		std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback,
		UINT style,
		HWND parent
	)
	{
		id = name_id++;
		wchar_t class_name[16];
		swprintf_s(class_name, L"class_%d", id);
	
		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(wc));

		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = _hInst;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = class_name;
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	
	
		// global callback function
		wc.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT 
		{
			Args args = arguments.get(hwnd);
			Window* window = (Window*)args[0];
			if (window == NULL)	return DefWindowProc(hwnd, msg, wParam, lParam);
	
			switch (msg)
			{
				case  WM_GETMINMAXINFO:
				{
					LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
					lpMMI->ptMinTrackSize.x = window->min_w;
					lpMMI->ptMinTrackSize.y = window->min_h;
	
					lpMMI->ptMaxTrackSize.x = window->max_w;
					lpMMI->ptMaxTrackSize.y = window->max_h;
					break;
				}
	
	
					// Keyboard
				case WM_SYSKEYDOWN:
					if (wParam == VK_ALT) Input::vk_alt = true;
					if (wParam == VK_F10) Input::vk_f10 = true;
					break;
	
				case WM_KEYDOWN:
					Input::__pressed_any = true;
					Input::__pressed[wParam] += 1;
					Input::__was_pressed[wParam] = true;
					break;
	
				case WM_KEYUP:
					Input::__pressed[wParam] = 0;
					break;
	
	
					// Mouse
				case WM_MOUSEMOVE:
				{
					float xPos = GET_X_LPARAM(lParam);
					float yPos = GET_Y_LPARAM(lParam);
					Mouse::pos_x = xPos / window->canvas.width;
					Mouse::pos_y = 1.0f - yPos / window->canvas.height;
				}break;
	
	
				// Mouse buttons down
				case WM_RBUTTONDOWN:
					Mouse::__pressed[WM_RBUTTONDOWN - 512] += 1;
					Mouse::__was_pressed[WM_RBUTTONDOWN - 512] = true;
					break;
	
				case WM_MBUTTONDOWN:
					Mouse::__pressed[WM_MBUTTONDOWN - 512] += 1;
					Mouse::__was_pressed[WM_MBUTTONDOWN - 512] = true;
					break;
	
				case WM_LBUTTONDOWN:
					Mouse::__pressed[WM_LBUTTONDOWN - 512] += 1;
					Mouse::__was_pressed[WM_LBUTTONDOWN - 512] = true;
					break;
	
				case WM_XBUTTONDOWN:
					Mouse::__pressed[WM_XBUTTONDOWN - 512] += 1;
					Mouse::__was_pressed[WM_XBUTTONDOWN - 512] = true;
					break;
	
	
				// Mouse buttons up
				case WM_MBUTTONUP:
					Mouse::__pressed[WM_MBUTTONDOWN - 512] = 0;
					break;
	
				case WM_RBUTTONUP:
					Mouse::__pressed[WM_RBUTTONDOWN - 512] = 0;
					break;
	
				case WM_XBUTTONUP:
					Mouse::__pressed[WM_XBUTTONDOWN - 512] = 0;
					break;
	
				case WM_LBUTTONUP:
					Mouse::__pressed[WM_LBUTTONDOWN - 512] = 0;
					break;
	
	
				case WM_SIZE:
					window->canvas.resize(hwnd);
					components.resize(hwnd);
					break;
			}
			
			// user's callback
			LRESULT res = args.callback(hwnd, msg, wParam, lParam, args);
			
			switch (msg)
			{
				case WM_CLOSE:
					delete window;
					return 0;
			}
	
			return res;
		};
	
	
		if (!RegisterClassEx(&wc))
		{
			gui::error_list.push_back(6);
			return false;
		}
	
		hwnd = CreateWindow(wc.lpszClassName, window_name.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, parent, (HMENU)0, (HINSTANCE)_hInst, NULL);
	
		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}
	
		hdc = GetDC(hwnd);
	
		// set window def ssize
		set_min_max_size(width, height, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	
		// add window to the widnow holder
		windows.push_back(std::make_pair(id, this));
	
		// put window ptr to global holder
		arguments.add(hwnd, this, NULL, callback);
	
		// repeat messages
		SendMessage(hwnd, WM_CREATE, 0, 0);
		SendMessage(hwnd, WM_SIZE, 0, 0);
		SendMessage(hwnd, WM_PAINT, 0, 0);
		return true;
	}
	
	Window::~Window()
	{
		auto pointer = std::find_if(windows.begin(), windows.end(),
					[=](const std::pair<int, Window*>& a) {return id == a.first; });
		windows.erase(pointer);
	
		ReleaseDC(hwnd, hdc);
		arguments.remove(hwnd);
		components.remove(hwnd);
		
		if (windows.size() == 0)
			PostQuitMessage(0);
	}
	
	void Window::close()
	{
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		SendMessage(hwnd, WM_DESTROY, 0, 0);
	}
	
	void Window::render_canvas(const PAINTSTRUCT& ps)
	{
		HDC hdc = GetDC(NULL);
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP hBmp = CreateHBITMAPfromByteArray(hdc, canvas.width, canvas.height, canvas.data);
		
		HGDIOBJ oldBMP = SelectObject(memDC, hBmp);
		
		const RECT& rect = ps.rcPaint;
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		
		// redraw area
		BitBlt(ps.hdc, rect.left, rect.top, width, height, memDC, rect.left, rect.top, SRCCOPY);
		
		SelectObject(memDC, oldBMP);
		DeleteDC(memDC);
		ReleaseDC(NULL, hdc);
	}
	
	void Window::render_canvas()
	{
		StretchDIBits(hdc, 0, 0, canvas.width, canvas.height, 0, 0, canvas.width, canvas.height, canvas.data, &canvas.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}
	
	void Window::redraw()
	{
		RedrawWindow(getHWND(), 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
	}
	
	int Window::height()
	{
		return canvas.height;
	}
	
	int Window::widht()
	{
		return canvas.width;
	}
	
	bool Window::valid()
	{
		return hwnd;
	}
	
	
	void Window::set_min_max_size(int minw, int minh, int maxw, int maxh)
	{
		min_w = max(0, minw);
		min_h = max(0, minh);
		
	
		if (maxw == MAX_WIN_SIZE)
			max_w = GetSystemMetrics(SM_CXSCREEN);
		else if (maxw != OLD_WIN_SIZE)
			max_w = maxw;
	
		if (maxh == MAX_WIN_SIZE)
			max_h = GetSystemMetrics(SM_CYSCREEN);
		else if (maxh != OLD_WIN_SIZE)
			max_h = maxh;
	}
	
	HWND Window::getHWND() { return hwnd; }
	

	//  ------------- static elements ---------------
	Window* Window::get_window(WindowId id)
	{
		auto pointer = std::find_if(windows.begin(), windows.end(), [id](const std::pair<int, Window*>& a) {return id == a.first; });
		return pointer != windows.end() ? pointer->second : NULL;
	}
	
	void Window::close(WindowId id)
	{
		Window* window = Window::get_window(id);
		if (window) SendMessage(window->hwnd, WM_CLOSE, 0, 0);
	}


	bool Window::is_running(WindowId id)
	{
		auto ptr = std::find_if(Window::windows.begin(), Window::windows.end(), 
										[id](const std::pair<WindowId, Window*>& pair) { return pair.first == id; });

		return ptr != Window::windows.end();
	}


	bool Window::is_running(Window* window)
	{
		auto ptr = std::find_if(Window::windows.begin(), Window::windows.end(),
			[window](const std::pair<WindowId, Window*>& pair) { return pair.second == window; });

		return ptr != Window::windows.end();
	}


	HWND Window::getHWND(WindowId id)
	{
		auto ptr = std::find_if(Window::windows.begin(), Window::windows.end(),
										[id](const std::pair<WindowId, Window*>& pair) { return pair.first == id; });

		HWND hwnd = ptr == Window::windows.end() ? 0 : ptr->second->hwnd;
		return hwnd;
	}
	
	
	void Window::default_msg_proc() {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	void Window::wait_msg_proc() {
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	// ---------------------------------------------------

	// set window static elems
	int Window::name_id = 0;
	std::vector<std::pair<int, Window*>> Window::windows = std::vector<std::pair<int, Window*>>();


	// create window
	WindowId create_window(const std::wstring& window_name,
		int width,
		int height,
		std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback,
		UINT style,
		HWND parent)
	{
		return (new Window(window_name, width, height, callback, style, parent))->id;
	}



	
	// ===================================  Window Components =================================================
	
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
		components.add(parent, comp);
		return true;
	}

	
	// =============== Radio Button ======================

	RadioButton::RadioButton() = default;
	RadioButton::RadioButton(const RadioButton& other)
	{ 
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
	}

	RadioButton& RadioButton::operator= (const RadioButton& other) 
	{ 
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
		return *this;
	}
	
	RadioButton::RadioButton(HWND parent, const std::wstring& text, float x, float y, float width, float height, UINT type, UINT style)
	{
		init(parent, text, x, y, width, height, type, style);
	}
	
	bool RadioButton::RadioButton::init(HWND parent, const std::wstring& text, float x, float y, float width, float height, UINT type, UINT style)
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
		components.add(parent, comp);
		return true;
	}
	
	int RadioButton::chosed()
	{
		return SendMessage(hwnd, BM_GETCHECK, 0, 0);
	}
	

	
	// ================ Check Button =================
	
	CheckBox::CheckBox() = default;
	CheckBox::CheckBox(const CheckBox& other) 
	{ 
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
	}
	
	CheckBox& CheckBox::operator= (const CheckBox& other) 
	{ 
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
		return *this;
	}
	
	CheckBox::CheckBox(HWND parent, const std::wstring& text, float x, float y, float width, float height, UINT type, UINT style)
	{
		init(parent, text, x, y, width, height, type, style);
	}
	
	bool CheckBox::init(HWND parent, const std::wstring& text, float x, float y, float width, float height, UINT type, UINT style)
	{
		this->parent = parent;
		this->text = text;
	
		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;
	
		hwnd = CreateWindow(L"Button", text.c_str(), style,
			x* nWidth, (1.0f - y - height) * nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, _hInst, NULL);
	
		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}
	
		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		components.add(parent, comp);
		return true;
	}
	
	bool CheckBox::checked()
	{
		return SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
	}


	// =================== ComboBox ========================
	
	ComboBox::ComboBox() = default;
	ComboBox::ComboBox(const ComboBox& other)
	{ 
		Component* comp = other.get();
		init(comp->parent, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
	}

	ComboBox& ComboBox::operator= (const ComboBox& other) 
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
			x* nWidth, (1.0f - y - height) * nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, _hInst, NULL);
	
		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}
	
		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		components.add(parent, comp);
		return true;
	}
	
	void ComboBox::add(const std::wstring& element)
	{
		// Add string to combobox.
		SendMessage(hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());
	
		// Send the CB_SETCURSEL message to display an initial item 
		//  in the selection field  
		SendMessage(hwnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	
	}
	
	void ComboBox::add(const std::vector<std::wstring>& elements)
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
	
	void ComboBox::remove(const std::wstring& name)
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
	Label::Label(const Label& other) 
	{ 
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
	}

	Label& Label::operator= (const Label& other) 
	{ 
		Component* comp = other.get();
		init(comp->parent, other.text, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
		return *this;
	}
	
	Label::Label(HWND parent, const std::wstring& text, float x, float y, float width, float height, UINT type, UINT style)
	{
		init(parent, text, x, y, width, height, type, style);
	}
	
	bool Label::init(HWND parent, const std::wstring& text, float x, float y, float width, float height, UINT type, UINT style)
	{
		this->parent = parent;
		this->text = text;
	
		RECT rect;
		GetClientRect(parent, &rect);
		int nWidth = rect.right - rect.left;
		int nHeight = rect.bottom - rect.top;
	
		hwnd = CreateWindow(L"static", L"label", style,
		x* nWidth, (1.0f - y - height) * nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, _hInst, NULL);
	
		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}
	
		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		components.add(parent, comp);
		SetWindowText(hwnd, text.c_str());
		return true;
	}
	
	void Label::set_text(const std::wstring& text)
	{
		SetWindowText(hwnd, text.c_str());
	}
	

	// ========================= Text ================================
	
	Text::Text() = default;
	Text::Text(const Text& other) 
	{ 
		Component* comp = other.get();
		init(comp->parent, comp->x, comp->y, comp->width, comp->height, comp->type, comp->style);
	}

	Text& Text::operator= (const Text& other) 
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
		x* nWidth, (1.0f - y - height) * nHeight, width* nWidth, height* nHeight, parent, (HMENU)id, _hInst, NULL);
	
		if (!hwnd)
		{
			gui::error_list.push_back(7);
			return false;
		}
	
		Component comp(id, x, (1.0f - y - height), width, height, type, style, hwnd, parent);
		components.add(parent, comp);
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
	
	void Text::set_text(const TCHAR* text)
	{
		SetWindowText(hwnd, text);
	}
	
	void Text::clear()
	{
		*text = '\0';
		SetWindowText(hwnd, NULL);
	}
		
	Text::~Text() {  ::operator delete(text); }
	
	
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
		components.add(parent, comp);
		return true;
	}
	
	void Table::create(const std::vector<std::wstring>& text_rows, const std::vector<std::wstring>& text_cols)
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
				cell_h = comp->height /(1 +  rows);
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
		components.add(parent, comp);
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