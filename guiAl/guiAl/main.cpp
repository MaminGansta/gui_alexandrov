#include "guiAlexandrov/include.h"



struct My_window : Window
{
	Image bg;

	Button btn;
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
					if (wParam == window->btn.id)
					{
						OutputDebugStringA("Pushed button\n");
					}
					else if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						if (LOWORD(wParam) == window->b.id)
						{
							MessageBox(hwnd, window->b.choosed().c_str(), NULL, NULL);
						}
					}
				}break;
				case WM_PAINT:
				{
					PAINTSTRUCT plug;
					BeginPaint(hwnd, &plug);
					draw_image(window->canvas, window->bg, 0.0f, 0.0f, 1.0f, 1.0f);
					window->render_canvas();

					EndPaint(hwnd, &plug);
				}return 0;

				case WM_CLOSE:
					quick_exit(0);

				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});



				btn.init(getHWND(), L"button", 0.1f, 0.1f, .1f, 0.1f, RESIZABLE);
				label.init(getHWND(), L"some text dsaf\nadsfadsfdasfdasf", 0.2f, 0.2f, 0.1f, 0.1f, RESIZABLE);
				set_font_size(btn.handle, 20);

				SetBkMode(GetDC(label.handle), TRANSPARENT);
				SetTextColor(GetDC(label.handle), RGB(255, 0, 0));


				text.init(getHWND(), 0.1f, 0.6f, 0.2f, 0.2f);

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

	My_window window;
	Window::wait_msg_proc();

	return 0;
}
