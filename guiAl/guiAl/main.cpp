#include "guiAlexandrov/include.h"

bool running = true;
#define BUTTON_ID 1
#define COMBO_1 10

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	al_init(hInstance);

	Timer timer(true);


	Image bg(L"back.png");

	Window win(L"window", 800, 600, DEF_STYLE, NULL, &bg, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		args arg = arguments.get(hwnd);
		if (arg[0] == NULL) return DefWindowProc(hwnd, msg, wParam, lParam);

		Window* window = (Window*)arg[0];
		Image* bg = (Image*)arg[1];

		switch (msg)
		{
		case WM_COMMAND:
		{
			if (wParam == BUTTON_ID)
			{
				OutputDebugStringA("Pushed button\n");
			}
			else if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				if (LOWORD(wParam) == COMBO_1)
				{
					std::wstring str = ComboBox::choosed(lParam);
					MessageBox(hwnd, str.c_str(), L"Message", MB_OK);
				}
			}
		}break;
		case WM_CLOSE:
		{
			running = false;
		}break;
		case WM_SIZE:
		{
			window->canvas.resize(hwnd);
			//window->resize_chileds();
		}break;
		case WM_PAINT:
		{
			PAINTSTRUCT plug;
			BeginPaint(hwnd, &plug);
			draw_image(window->canvas, *bg, 0.0f, 0.0f, 1.0f, 1.0f);
			window->render_canvas();
			EndPaint(hwnd, &plug);
		}break;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	});

	Button btn(L"button", win.getHWND(), BUTTON_ID);
	Label label(win.getHWND(), 0, L"some text", 200, 200, 100);
	Text text(win.getHWND(), 5, 400, 200, 200, 200);

	ComboBox b(win.getHWND(), COMBO_1);
	std::vector<std::wstring> combo_elements{ L"Mercury", L"Venus", L"Earth", L"Mars", L"Jupiter", L"Saturn", L"Uranus", L"NOT_Neptune" };
	b.add(combo_elements);

	CheckBox(win.getHWND(), L"Check", 11, 400, 30);
	CheckBox(win.getHWND(), L"Check2", 12, 400, 50);

	RadioButton(win.getHWND(), L"Radio", 13, 400, 80);
	RadioButton(win.getHWND(), L"Radio2", 14, 400, 100);

	
	while (running)
	{
		Window::default_msg_proc();
		timer.update();
	}

	return 0;
}
