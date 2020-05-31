#pragma once

#include <cassert>
#include <memory.h>
#include "color.h"

#include "../error_handler/errors.h"


namespace gui
{
	// ============= standart image ==================

	struct Image
	{
		int height = 0, width = 0;
		Color* data = NULL;

		Image();
		Image(const wchar_t* filename_utf8);

		Image(const Image& copy);

		Image(Image&& other);

		Image(int width, int height);

		~Image();

		Image& operator= (const Image& copy);

		Image& operator = (Image&& other);


		bool open(const wchar_t* filename_utf8);

		bool valid() const;

		void resize(int width, int height);


		Color& operator [] (int idx);

		const Color& operator [] (int idx) const;


		Color& get_pixel(int x, int y);

		Color& get_pixel_scaled(int x, int y, int screen_w, int screen_h);

		const Color& get_pixel_scaled(int x, int y, int screen_w, int screen_h) const;

	};


	// =============== float Image  ==================

	struct fImage
	{
		int height = 0, width = 0;
		fColor* data = NULL;

		fImage();
		fImage(const wchar_t* filename_utf8);

		fImage(int width, int height);

		fImage(const fImage& copy);

		fImage(fImage&& other);

		fImage(const Image& image);

		~fImage();

		fImage& operator = (const fImage& copy);

		fImage& operator = (fImage&& other);


		bool open(const wchar_t* filename_utf8);

		bool valid();

		void resize(int width, int height);


		fColor& operator [] (int idx);

		const fColor& operator [] (int idx) const;


		fColor& get_pixel(int x, int y);

		fColor& get_pixel_scaled(int x, int y, int screen_w, int screen_h);

		const fColor& get_pixel_scaled(int x, int y, int screen_w, int screen_h) const;

		operator Image();
	};

}