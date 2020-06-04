

#include "image/image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include "stb_image.h"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


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


	Image_base<uint8_t> BGRA_2_RBGA(const Image_base<uint8_t>& image)
	{
		Image_base<uint8_t> temp(image.width, image.height);

		for (int i = 0; i < image.height; i++)
		{
			for (int j = 0; j < image.width; j++)
			{
				const Color& color= image[i * image.width + j];
				temp[i * image.width + j] = Color(color.b, color.g, color.r, color.a);
			}
		}
		return temp;
	}

	
	
	int write_image(char* filename, const Image_base<uint8_t>& image, int type, int flip, int png_qulity)
	{
		stbi_flip_vertically_on_write(flip);

		switch (type)
		{
			case PNG:
				strcat_s(filename, 255, ".png");
				return stbi_write_png(filename, image.width, image.height, 4, image.data, image.width * sizeof(int));
			case BMP:
				strcat_s(filename, 255, ".bmp");
				return stbi_write_bmp(filename, image.width, image.height, 4, image.data);
			case TGA:
				strcat_s(filename, 255, ".tga");
				return stbi_write_tga(filename, image.width, image.height, 4, image.data);
			case JPG:
				strcat_s(filename, 255, ".jpg");
				return stbi_write_jpg(filename, image.width, image.height, 4, image.data, png_qulity);
			//case HDR:
			//	return stbi_write_hdr(filename, image.width, image.height, 4, const float* data);
		}
		return 0;
	}


	template struct Image_type<uint8_t>;
	template struct Image_type<float>;

}