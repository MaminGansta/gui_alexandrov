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
		Args args = arguments.get(hwnd);
		if (args[0] == NULL) return DefWindowProc(hwnd, msg, wParam, lParam);

		Window* window = (Window*)args[0];
		Image* bg = (Image*)args[1];

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
			components.update(hwnd);
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

	Button btn(L"button", BUTTON_ID, 0.1f, 0.1f, .1f, 0.1f, win.getHWND());
	Label label(win.getHWND(), L"some text", 0, 0.2f, 0.2f, 0.1f, 0.1f, RESIZABLE);
	Text text(win.getHWND(), 5, 0.4f, 0.2f, 0.2f, 0.2f);

	ComboBox b(win.getHWND(), COMBO_1, 0.3f, 0.2f);
	std::vector<std::wstring> combo_elements{ L"Mercury", L"Venus", L"Earth", L"Mars", L"Jupiter", L"Saturn", L"Uranus", L"NOT_Neptune" };
	b.add(combo_elements);

	//CheckBox(win.getHWND(), L"Check", 11, 400, 30);
	//CheckBox(win.getHWND(), L"Check2", 12, 400, 50);
	//
	//RadioButton(win.getHWND(), L"Radio", 13, 400, 80);
	//RadioButton(win.getHWND(), L"Radio2", 14, 400, 100);
	
	while (running)
	{
		Window::default_msg_proc();
		timer.update();
	}

	return 0;
}
