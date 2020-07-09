
#include "guiAlexandrov.h"


template <typename Image_type>
struct My_window : gui::Window
{
	Image_type bg;

	gui::Button btn;
	gui::Button bClear;
	std::vector<int> windows;


	gui::Label label;
	gui::Text text;
	gui::ComboBox b;
	gui::CheckBox aa;
	gui::CheckBox aac;
	gui::RadioButton aaa;
	gui::RadioButton aaav;

	My_window(Image_type& bg) : bg(bg)
	{

		init(L"window", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				My_window* window = (My_window*)args[0];

				switch (msg)
				{
				case WM_COMMAND:
				{
					if (LOWORD(wParam) == window->btn.id)
						window->windows.push_back(gui::image_window(window->bg));

					if (LOWORD(wParam) == window->bClear.id)
					{
						for (int id : window->windows)
							Window::close(id);
					
						window->windows.clear();
					}
				}break;

				case WM_SIZE:
				{
					gui::cpu::draw_image(window->canvas, window->bg, 0, 0, 1.0f, 1.0f);

					gui::cpu::draw_filled_circle_a(window->canvas, 0.8f, 0.95f, gui::Color(255), 0.05f);
					gui::cpu::draw_filled_circle_a(window->canvas, 0.9f, 0.95f, gui::Color(255, 255, 255, 50), 0.05f);

					gui::cpu::draw_filled_rect(window->canvas, 0.775f, 0.85f, 0.05f, 0.05f, gui::Color(255));
					gui::cpu::draw_filled_rect_a(window->canvas, 0.875f, 0.85f, 0.05f, 0.05f, gui::Color(255, 255, 255, 100));

					gui::cpu::draw_line(window->canvas, 0.75f, 0.8f, 0.95f, 0.81f, gui::Color(255));
					gui::cpu::draw_line(window->canvas, 0.75f, 0.75f, 0.95f, 0.76f, gui::Color(255), 3);
					gui::cpu::draw_line_a(window->canvas, 0.75f, 0.7f, 0.95f, 0.71f, gui::Color(255, 255, 255, 100), 5);
					gui::cpu::draw_line_a(window->canvas, 0.75f, 0.65f, 0.95f, 0.66f, gui::Color(255, 255, 255, 50), 7);

					gui::render_text_ml(window->canvas, 0.75, 0.6f, L"Text rendering \nMulti line", gui::Color(255), 1.2f, gui::get_def_font(25));
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


			btn.init(getHWND(), L"button", 0.1f, 0.1f, 0.1f, 0.1f);
			bClear.init(hwnd, L"Clear", 0.01f, 0.8f, 0.1f, 0.1f);
			
			label.init(getHWND(), L"some text dsaf\nadsfadsfdasfdasf", 0.2f, 0.2f, 0.1f, 0.1f, RESIZABLE, DEF_LABEL);
			gui::set_font_size(btn.hwnd, 20);
			
			
			text.init(getHWND(), 0.1f, 0.5f, 0.2f, 0.2f, RESIZABLE, DEF_TEXT | WS_VSCROLL | WS_HSCROLL);
			
			b.init(getHWND(), 0.4f, 0.2f);
			std::vector<std::wstring> combo_elements{ L"Mercury", L"Venus", L"Earth", L"Mars", L"Jupiter", L"Saturn", L"Uranus", L"NOT_Neptune" };
			b.add(combo_elements);
			
			aa.init(getHWND(), L"Check", 0.5f, 0.3f);
			aac.init(getHWND(), L"Check2", 0.5f, 0.4f);
			
			aaa.init(getHWND(), L"Radio", 0.4f, 0.6f, 0.1f, 0.1f);
			aaav.init(getHWND(), L"Radio2", 0.4f, 0.7f, 0.1f, 0.1f);
	}

};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	gui::init(hInstance);

	gui::Image bg(L"bg.jpg");
	if (!bg.valid()) return -1;

	My_window<gui::Image>* window = new My_window<gui::Image>(bg);
	gui::console::printf(L"hello world %f\n", get_time());

	gui::Window::wait_msg_proc();
	return 0;
}