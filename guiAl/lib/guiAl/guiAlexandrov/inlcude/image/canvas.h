#pragma once


#include <Windows.h>
#include <cassert>

#include "Image_base.h"


namespace gui
{

	struct Canvas : Image_base<uint8_t>
	{
		int capacity = 0;

		BITMAPINFO bitmap_info;

		Canvas();

		~Canvas();

		void resize(int width, int height);

		void set_max_buffer_size();

		Color& operator [] (int inx);

		void reserve(int capacity);
	};

}