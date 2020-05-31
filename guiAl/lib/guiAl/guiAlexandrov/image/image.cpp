

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

	bool Image::valid() const
	{
		return width != 0 && height != 0 && data != NULL;
	}

	void Image::resize(int width, int height)
	{
		this->width = width;
		this->height = height;
		delete[] data;
		data = new Color[width * height];
	}

	Image::Image(int width, int height) : width(width), height(height)
	{
		data = new Color[width * height];
	}

	Color& Image::get_pixel(int x, int y)
	{
		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	Color& Image::operator [] (int idx)
	{
		assert((uint32_t)idx < width* height);
		return data[idx];
	}

	const Color& Image::operator [] (int idx) const
	{
		assert((uint32_t)idx < width* height);
		return data[idx];
	}

	Color& Image::get_pixel_scaled(int x, int y, int screen_w, int screen_h)
	{
		y = y * height / screen_h;
		x = x * width / screen_w;

		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}


	const Color& Image::get_pixel_scaled(int x, int y, int screen_w, int screen_h) const
	{
		y = y * height / screen_h;
		x = x * width / screen_w;

		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

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

	bool fImage::valid()
	{
		return width != 0 && height != 0 && data != NULL;
	}


	fImage::fImage(int width, int height) : width(width), height(height)
	{
		data = new fColor[width * height];
	}

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


	void fImage::resize(int width, int height)
	{
		this->width = width;
		this->height = height;
		delete[] data;
		data = new fColor[width * height];
	}

	fColor& fImage::get_pixel(int x, int y)
	{
		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	fColor& fImage::operator[] (int idx)
	{
		assert((uint32_t)idx < width* height);
		return data[idx];
	}

	const fColor& fImage::operator[] (int idx) const
	{
		assert((uint32_t)idx < width* height);
		return data[idx];
	}


	fColor& fImage::get_pixel_scaled(int x, int y, int screen_w, int screen_h)
	{
		y = y * height / screen_h;
		x = x * width / screen_w;

		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	const fColor& fImage::get_pixel_scaled(int x, int y, int screen_w, int screen_h) const
	{
		y = y * height / screen_h;
		x = x * width / screen_w;

		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	fImage::operator Image()
	{
		Image res(width, height);
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				res[i * width + j] = data[i * width + j];
		return res;
	}

}