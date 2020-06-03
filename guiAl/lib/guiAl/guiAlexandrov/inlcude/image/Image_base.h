#pragma once


#include "image/color.h"


namespace gui
{

	template <typename T>
	struct Image_base
	{
		Color_base<T>* data = nullptr;
		int height = 0, width = 0;
		int whole_size = 0;

		Image_base() = default;

		Image_base(int width, int height)
		:
			width(width),
			height(height),
			whole_size(height* width)
		{}
		
		bool valid() const
		{
			return width != 0 && height != 0 && data != NULL;
		}

		void resize(int width, int height)
		{
			this->width = width;
			this->height = height;
			delete[] data;
			data = new Color_base<T>[width * height];
		}

		Color_base<T>& get_pixel(int x, int y)
		{
			assert(((uint32_t)y < height) | ((uint32_t)x < width));
			return data[y * width + x];
		}

		Color_base<T>& operator[] (int idx)
		{
			assert((uint32_t)idx < whole_size);
			return data[idx];
		}

		const Color_base<T>& operator[] (int idx) const
		{
			assert((uint32_t)idx < whole_size);
			return data[idx];
		}


		Color_base<T>& get_pixel_scaled(int x, int y, int screen_w, int screen_h)
		{
			y = y * height / screen_h;
			x = x * width / screen_w;

			assert(((uint32_t)y < height) | ((uint32_t)x < width));
			return data[y * width + x];
		}

		const Color_base<T>& get_pixel_scaled(int x, int y, int screen_w, int screen_h) const
		{
			y = y * height / screen_h;
			x = x * width / screen_w;

			assert(((uint32_t)y < height) | ((uint32_t)x < width));
			return data[y * width + x];
		}
	};

}