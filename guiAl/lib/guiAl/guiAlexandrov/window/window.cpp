
#include "window/window.h"


// =========================================== CALLBACK ARGUMENTS ========================================================

// Args
void* Args::operator [] (int i) { return vals[i]; }

namespace gui
{
	//Arguments
	void Arguments::add(HWND hwnd, void* pwindow, void* parg, std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)> callback)
	{
		buffer.push_back(std::make_pair(hwnd, Args{ pwindow, parg, callback }));
	}

	Args Arguments::get(HWND hwnd)
	{
		auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, Args> in) { return hwnd == in.first; });
		return it == buffer.end() ? Args{ NULL,NULL } : it->second;
	}

	void Arguments::remove(HWND hwnd)
	{
		auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, Args> in) { return hwnd == in.first; });
		if (it != buffer.end())
			buffer.erase(it);
	}

	// global
	Arguments _arguments;


	// =========================================== WINDOW _components LAYOUT ====================================================
	
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
	Component_crt _components;
	

	// Component_id

	// global
	int _global_id = 0;
	
	Component_id::Component_id() : id(_global_id++) {}
	
	Component* Component_id::get() const
	{
		// this is how c++ work
		int comp_id = id;
		auto& comps = _components[parent];
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
		id = _name_id++;
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
			Args args = _arguments.get(hwnd);
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
					window->canvas.resize(LOWORD(lParam), HIWORD(lParam));
					_components.resize(hwnd);
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
		_arguments.add(hwnd, this, NULL, callback);
	
		// repeat messages
		SendMessage(hwnd, WM_CREATE, 0, 0);
		SendMessage(hwnd, WM_SIZE, 0, MAKEWPARAM(width, height));
		SendMessage(hwnd, WM_PAINT, 0, 0);
		return true;
	}
	
	Window::~Window()
	{
		auto pointer = std::find_if(windows.begin(), windows.end(),
					[=](const std::pair<int, Window*>& a) {return id == a.first; });
		windows.erase(pointer);
	
		ReleaseDC(hwnd, hdc);
		_arguments.remove(hwnd);
		_components.remove(hwnd);
		
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
		
		DeleteObject(hBmp);
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
	
	int Window::width()
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
	int Window::_name_id = 0;
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


}