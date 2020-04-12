
template <typename Image_type>
struct Image_window : Window
{
	Image_type image;

	Image_window(const Image_type& image, int flags = DEF_WINDOW) : image(image)
	{
		init(L"window", image.width, image.height, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Image_window* window = (Image_window*)args[0];

				switch (msg)
				{
					case WM_PAINT:
					{
						draw_image(window->canvas, window->image, 0.0f, 0.0f, 1.0f, 1.0f);
						window->render_canvas();
					}break;
				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});

		if (flags == 0)
		{
			SetWindowLong(handle, GWL_STYLE, 0); //remove all window styles, check MSDN for details
			ShowWindow(handle, SW_SHOW);
		}
	}
};

template <typename Image_type>
int image_window(const Image_type& image, int flags = DEF_WINDOW)
{
	return (new Image_window<Image_type>(image, flags))->class_id;
}

int image_window(const wchar_t* name, int flags = DEF_WINDOW)
{
	return (new Image_window<Image>(name, flags))->class_id;
}