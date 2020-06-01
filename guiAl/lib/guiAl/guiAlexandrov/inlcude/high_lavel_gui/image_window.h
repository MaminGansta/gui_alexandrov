#pragma once

#include "../win_api/window.h"
#include "../image/image.h"
#include "../image/draw.h"


namespace gui
{
	template <typename Image_type>
	struct Image_window : Window
	{
		Image_type image;

		Image_window(const Image_type& image, int flags = DEF_WINDOW);
		
	};

	
	// create from Image
	template <typename Image_type>
	int image_window(const Image_type& image, int flags = DEF_WINDOW)
	{
		return (new Image_window<Image_type>(image, flags))->id;
	}

	
	// Create by image name
	int image_window(const wchar_t* name, int flags = DEF_WINDOW);
}