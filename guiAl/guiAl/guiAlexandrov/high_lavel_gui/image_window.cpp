
template <typename Image_type>
struct Image_window : Window
{
	Image_type* image;

	Image_window(Image_type& image) : image(&image)
	{
		init(L"window", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Image_window* window = (Image_window*)args[0];

				switch (msg)
				{
					case WM_PAINT:
					{
						PAINTSTRUCT plug;
						BeginPaint(hwnd, &plug);
						draw_image(window->canvas, *window->image, 0.0f, 0.0f, 1.0f, 1.0f);
						window->render_canvas();
						EndPaint(hwnd, &plug);
					}break;
					case WM_CLOSE:
					{
						safe_release(window);
					}break;
				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});
	}
};