
#include "high_lavel_gui/image_window.h"


namespace gui
{

	template <typename Image_type>
	Image_window<Image_type>::Image_window(const Image_type& image, int flags) : image(image)
	{
		init(L"window", image.width, image.height, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Image_window* window = (Image_window*)args[0];

				switch (msg)
				{
					case WM_SIZE:
					{
						gui::cpu::draw_image(window->canvas, window->image, 0.0f, 0.0f, 1.0f, 1.0f);
					}break;

					case WM_PAINT:
					{
						PAINTSTRUCT ps;
						BeginPaint(hwnd, &ps);
						window->render_canvas(ps);
						EndPaint(hwnd, &ps);
					}break;
				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});

		set_min_max_size(0, 0);
	}



	int image_window(const wchar_t* name, int flags)
	{
		return (new Image_window<Image>(name, flags))->id;
	}

}