

#include "image/image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include "stb_image.h"


namespace gui
{
	
	int convert_wchar_to_utf8(char* buffer, size_t buffer_len, const wchar_t* output)
	{
		return stbi_convert_wchar_to_utf8(buffer, buffer_len, output);
	}

	uint8_t* load_image(const char* filename, int* x, int* y, int* comp, int req_comp)
	{
		return stbi_load(filename, x, y, comp, req_comp);
	}


	void image_free(void* ptr)
	{
		stbi_image_free(ptr);
	}


	template struct Image_type<uint8_t>;
	template struct Image_type<float>;

}