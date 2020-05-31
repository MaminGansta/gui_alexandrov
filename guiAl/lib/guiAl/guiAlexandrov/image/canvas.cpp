

#include "image/canvas.h"


	Canvas::Canvas()
	{
#ifdef FULL_SCREAN_CANVAS
		capacity = GetSystemMetrics(SM_CXSCREEN) * GetSystemMetrics(SM_CYSCREEN);
		data = new Color[capacity];
#endif
	}

	Canvas::~Canvas() {
		delete[] data;
	}

	void Canvas::resize(HWND hwnd)
	{
		RECT rect;
		GetClientRect(hwnd, &rect);

		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		whole_size = width * height;

		bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
		bitmap_info.bmiHeader.biWidth = width;
		bitmap_info.bmiHeader.biHeight = height;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biCompression = BI_RGB;

		
		if (whole_size < capacity) return;

		capacity = width * height;
		delete[] data;
		data = new Color[capacity];
	}

	Color& Canvas::operator [] (int inx)
	{
		assert((uint32_t)inx < whole_size);
		return data[inx];
	}

	void Canvas::reserve(int capacity)
	{
		this->capacity = capacity;
		delete[] data;
		data = new Color[capacity];
	}