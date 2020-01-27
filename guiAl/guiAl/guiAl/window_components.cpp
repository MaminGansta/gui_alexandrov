

struct Component
{
	int x, y, winth, height;
	int old_parent_w, old_parent_h;
	HWND handle;
};



struct Button
{
	HWND handle;

	Button(
		const WCHAR* button_name,
		HWND parent,
		int id,
		int x = 10,
		int y = 10,
		int width = 100,
		int height = 20,
		UINT style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_PUSHBUTTON
	)
	{
		handle = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			button_name, // Button text 
			style,     // Styles 
			x,         // x position 
			y,         // y position 
			width,     // Button width
			height,    // Button height
			parent,    // Parent window
			(HMENU)id, // menu.
			hInst,
			NULL);      // Pointer not needed.
	}

	~Button() { DestroyWindow(handle); };
};