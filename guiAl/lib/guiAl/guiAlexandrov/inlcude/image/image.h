#pragma once

#include <cassert>
#include <memory.h>

#include "../error_handler/errors.h"
#include "Image_base.h"


namespace gui
{

	// ============= standart image ==================

	struct Image : Image_base<uint8_t>
	{

		Image();
		Image(const wchar_t* filename_utf8);

		Image(const Image& copy);

		Image(Image&& other);

		Image(int width, int height);

		~Image();

		Image& operator= (const Image& copy);

		Image& operator = (Image&& other);

		bool open(const wchar_t* filename_utf8);
	};


	// =============== float Image  ==================

	struct fImage : Image_base<float>
	{

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
		
		operator Image();
	};

}