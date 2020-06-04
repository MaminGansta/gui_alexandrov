

#include "image/canvas.h"


namespace gui
{

	Canvas::Canvas() = default;

	void Canvas::resize(int width, int height)
	{
		this->width = width;
		this->height = height;
		whole_size = width * height;

		bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
		bitmap_info.bmiHeader.biWidth = width;
		bitmap_info.bmiHeader.biHeight = height;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biCompression = BI_RGB;


		if (whole_size > capacity)
		{
			capacity = width * height;
			delete[] data;
			data = new Color[capacity];
		}
	}

	void Canvas::set_max_buffer_size()
	{
		delete[] data;
		capacity = GetSystemMetrics(SM_CXSCREEN) * GetSystemMetrics(SM_CYSCREEN);
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

}