#define MAX_THREADS 8
#include "guiAlexandrov/include.h"


template <typename Image_type>
struct My_window : Window
{
	Image_type bg;

	Button btn;
	Button bClear;
	std::vector<int> windows;


	Label label;
	Text text;
	ComboBox b;
	CheckBox aa;
	CheckBox aac;
	RadioButton aaa;
	RadioButton aaav;

	My_window()
	{
		bg.open(L"back.png");

		init(L"window", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				My_window* window = (My_window*)args[0];

				switch (msg)
				{
					case WM_COMMAND:
					{
						if (LOWORD(wParam) == window->btn.id)
							window->windows.push_back(image_window(window->bg));

						if (LOWORD(wParam) == window->bClear.id)
						{
							for (int id : window->windows)
								Window::close(id);
						
							window->windows.clear();
						}
					}break;
					case WM_SIZE:
					{

						draw_image_async(window->canvas, window->bg, 0.0f, 0.0f, 1.0f, 1.0f);
						draw_image_async_a(window->canvas, window->bg, 0.5f, 0.5f, 0.5f, 0.5f, 0.3f);

						draw_filled_rect_async_a(window->canvas, 0, 0, 0.5f, 0.5f, Color(255, 0, 0, 50));
						draw_rect(window->canvas, 0, 0, 0.5f, 0.5f, Color(255, 0, 0), 2);

						draw_rect_a(window->canvas, 0.5f, 0.5f, 0.5f, 0.5f, Color(0, 0, 0, 30), 2);
						draw_filled_circle_a(window->canvas, 0.1f, 0.1f, Color(0, 0, 255, 150), 0.1f);

						render_text(window->canvas, 0.6f, 0.8f, L"S1MPLE TEXT (rendering)", Color(), get_def_font(25));

					}break;

					case WM_PAINT:
					{
						PAINTSTRUCT ps;
						BeginPaint(hwnd, &ps);

						window->render_canvas(ps.rcPaint);
						EndPaint(hwnd, &ps);
					}break;
				}
				return DefWindowProc(hwnd, msg, wParam, lParam);
			});


			btn.init(getHWND(), L"button", 0.1f, 0.1f, 0.1f, 0.1f, RESIZABLE);
			bClear.init(hwnd, L"Clear", 0.01f, 0.8f, 0.1f, 0.1f, RESIZABLE);
			
			label.init(getHWND(), L"some text dsaf\nadsfadsfdasfdasf", 0.2f, 0.2f, 0.1f, 0.1f, RESIZABLE, DEF_LABEL);
			set_font_size(btn.hwnd, 20);
			
			
			text.init(getHWND(), 0.1f, 0.6f, 0.2f, 0.2f, RESIZABLE, DEF_TEXT | WS_VSCROLL | WS_HSCROLL);
			
			b.init(getHWND(), 0.4f, 0.2f);
			std::vector<std::wstring> combo_elements{ L"Mercury", L"Venus", L"Earth", L"Mars", L"Jupiter", L"Saturn", L"Uranus", L"NOT_Neptune" };
			b.add(combo_elements);
			
			aa.init(getHWND(), L"Check", 0.5f, 0.3f);
			aac.init(getHWND(), L"Check2", 0.5f, 0.4f);
			
			aaa.init(getHWND(), L"Radio", 0.4f, 0.6f, 0.1f, 0.1f, RESIZABLE);
			aaav.init(getHWND(), L"Radio2", 0.4f, 0.7f, 0.1f, 0.1f, RESIZABLE);
	}

};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	al_init(hInstance);

	My_window<Image>* window = new My_window<Image>();

	Window::wait_msg_proc();
	return 0;
}
