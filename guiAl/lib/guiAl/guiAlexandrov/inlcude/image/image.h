#pragma once

#include <cassert>
#include <memory.h>

#include "../error_handler/errors.h"
#include "Image_base.h"


namespace gui
{

	int convert_wchar_to_utf8(char* buffer, size_t buffer_len, const wchar_t* output);
	uint8_t* load_image(const char* filename, int* x, int* y, int* comp, int req_comp);
	void image_free(void* ptr);

	
	template <typename T>
	struct Image_type : Image_base<T>
	{

		Image_type() = default;
		Image_type(const wchar_t* filename_utf8)
		{
			open(filename_utf8);
		}

		Image_type(const Image_base<T>& other)
		:
			Image_base<T>(other)
		{}

		Image_type(Image_base<T>&& other)
		:
			Image_base<T>(other)
		{}


		Image_type(int width, int height) :
			Image_base<T>(width, height) {}


		Image_type<T>& operator= (const Image_base<T>& copy)
		{
			delete[] this->data;
			this->height = copy.height;
			this->width = copy.width;
			this->data = new Color_base<T>[this->height * this->width];
			memmove(this->data, copy.data, sizeof(Color_base<T>) * this->height * this->width);
			return *this;
		}

		Image_type<T>& operator = (Image_base<T>&& other)
		{
			delete[] this->data;
			this->data = other.data;
			this->width = other.width;
			this->height = other.height;
			other.data = NULL;
			return *this;
		}

		bool open(const wchar_t* filename_utf8)
		{
			delete[] this->data;

			int chanels;
			char filename[256];
			convert_wchar_to_utf8(filename, sizeof(filename), filename_utf8);

			uint8_t* raw = load_image(filename, &this->width, &this->height, &chanels, 0);

			if (raw == NULL)
			{
				error_list.push_back(8);
				return false;
			}

			this->whole_size = this->width * this->height;
			this->data = new Color_base<T>[this->whole_size];

			if (chanels == 3)
			{
				for (int y = 0; y < this->height; y++)
				{
					for (int x = 0; x < this->width; x++)
					{
						int pos = (y * this->width + x) * chanels;
						this->data[(this->height - y - 1) * this->width + x] = Color_base<T>(raw[pos], raw[pos + 1], raw[pos + 2]);
					}
				}
			}
			else if (chanels == 4)
			{
				for (int y = 0; y < this->height; y++)
				{
					for (int x = 0; x < this->width; x++)
					{
						int pos = (y * this->width + x) * chanels;
						this->data[(this->height - y - 1) * this->width + x] = Color_base<T>(raw[pos], raw[pos + 1], raw[pos + 2], raw[pos + 3]);
					}
				}
			}

			image_free(raw);
			return true;
		}


		operator Image_type<uint8_t>()
		{
			Image_type<uint8_t> temp(this->width, this->height);

			for (int i = 0; i < this->height; i++)
				for (int j = 0; j < this->width; j++)
					temp[i * this->width + j] = this->data[i * this->width + j];

			return temp;
		}

		operator Image_type<float>()
		{
			Image_type<float> temp(this->width, this->height);

			for (int i = 0; i < this->height; i++)
				for (int j = 0; j < this->width; j++)
					temp[i * this->width + j] = this->data[i * this->width + j];

			return temp;
		}

	};

	typedef Image_type<float> fImage;
	typedef Image_type<uint8_t> Image;

	extern template struct Image_type<uint8_t>;
	extern template struct Image_type<float>;

}