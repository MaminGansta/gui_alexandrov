

#include "image/image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include "stb_image.h"


namespace gui
{
	// ============= standart image ==================

	Image::Image() = default;
	Image::Image(const wchar_t* filename_utf8)
	{
		open(filename_utf8);
	}

	Image::Image(const Image& copy)
	{
		height = copy.height;
		width = copy.width;
		data = new Color[height * width];
		memmove(data, copy.data, sizeof(Color) * height * width);
	}

	Image::Image(Image&& other)
	{
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
	}

	Image& Image::operator= (const Image& copy)
	{
		delete[] data;
		height = copy.height;
		width = copy.width;
		data = new Color[height * width];
		memmove(data, copy.data, sizeof(Color) * height * width);
		return *this;
	}

	Image& Image::operator = (Image&& other)
	{
		delete[] data;
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		return *this;
	}


	bool Image::open(const wchar_t* filename_utf8)
	{
		delete[] data;

		int chanels;
		char filename[256];
		stbi_convert_wchar_to_utf8(filename, sizeof(filename), filename_utf8);

		uint8_t* raw = stbi_load(filename, &width, &height, &chanels, 0);

		if (raw == NULL)
		{
			error_list.push_back(8);
			return false;
		}

		data = new Color[width * height];

		if (chanels == 3)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int pos = (y * width + x) * chanels;
					data[(height - y - 1) * width + x] = Color(raw[pos], raw[pos + 1], raw[pos + 2]);
				}
			}
		}
		else if (chanels == 4)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int pos = (y * width + x) * chanels;
					data[(height - y - 1) * width + x] = Color(raw[pos], raw[pos + 1], raw[pos + 2], raw[pos + 3]);
				}
			}
		}

		stbi_image_free(raw);
		return true;
	}


	Image::Image(int width, int height) : 
		Image_base<uint8_t>(width, height) {}


	Image::~Image() { delete[] data; }


	// =============== float Image  ==================


	fImage::fImage() = default;
	fImage::fImage(const wchar_t* filename_utf8)
	{
		open(filename_utf8);
	}

	bool fImage::open(const wchar_t* filename_utf8)
	{
		delete[] data;

		int chanels;
		char filename[256];
		stbi_convert_wchar_to_utf8(filename, sizeof(filename), filename_utf8);

		uint8_t* raw = stbi_load(filename, &width, &height, &chanels, 0);

		if (raw == NULL)
		{
			error_list.push_back(8);
			return false;
		}

		data = new fColor[width * height];

		if (chanels == 3)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int pos = (y * width + x) * chanels;
					data[(height - y - 1) * width + x] = fColor(raw[pos], raw[pos + 1], raw[pos + 2]);
				}
			}
		}
		else if (chanels == 4)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int pos = (y * width + x) * chanels;
					data[(height - y - 1) * width + x] = fColor(raw[pos], raw[pos + 1], raw[pos + 2], raw[pos + 3]);
				}
			}
		}

		stbi_image_free(raw);
		return true;
	}


	fImage::fImage(int width, int height) :
		Image_base<float>(width, height) {}

	fImage::fImage(const fImage& copy)
	{
		height = copy.height;
		width = copy.width;
		data = new fColor[height * width];
		memmove(data, copy.data, sizeof(fColor) * height * width);
	}

	fImage::fImage(fImage&& other)
	{
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
	}

	fImage::fImage(const Image& image)
	{
		resize(image.width, image.height);
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				data[i * width + j] = image[i * width + j];
	}

	fImage& fImage::operator= (const fImage& copy)
	{
		delete[] data;
		height = copy.height;
		width = copy.width;
		data = new fColor[height * width];
		memmove(data, copy.data, sizeof(fColor) * height * width);
		return *this;
	}

	fImage& fImage::operator = (fImage&& other)
	{
		delete[] data;
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		return *this;
	}

	fImage::~fImage() { delete[] data; }



}