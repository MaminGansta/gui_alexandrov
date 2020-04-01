#include "guiAlexandrov/include.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	al_init(hInstance);



	Image bg(L"back.png");

	Window win(L"window", 800, 600, [&bg](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
	{
		Window* window = (Window*)args[0];

		switch (msg)
		{
		case WM_COMMAND:
		{
			//if (wParam == BUTTON_ID)
			//{
			//	OutputDebugStringA("Pushed button\n");
			//	SendMessage(hwnd, WM_PAINT, 0, 0);
			//}
			//else if (HIWORD(wParam) == CBN_SELCHANGE)
			//{
			//	if (LOWORD(wParam) == COMBO_1)
			//	{
			//		
			//	}
			//}
		}break;
		case WM_PAINT:
		{
			PAINTSTRUCT plug;
			BeginPaint(hwnd, &plug);
			draw_image(window->canvas, bg, 0.0f, 0.0f, 1.0f, 1.0f);
			window->render_canvas();

			EndPaint(hwnd, &plug);
		}return 0;

		case WM_CLOSE:
			quick_exit(0);

		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	});




	Button btn(win.getHWND(), L"button",  0.1f, 0.1f, .1f, 0.1f, RESIZABLE);
	Label label(win.getHWND(), L"some text dsaf\nadsfadsfdasfdasf", 0.2f, 0.2f, 0.1f, 0.1f, RESIZABLE);
	set_font_size(btn.handle, 20);

	//SetBkMode(GetDC(label.handle), TRANSPARENT);
	//SetTextColor(GetDC(label.handle), RGB(255, 0, 0));
	
	
	Text text(win.getHWND(), 0.1f, 0.6f, 0.2f, 0.2f);

	ComboBox b(win.getHWND(), 0.4f, 0.2f);
	std::vector<std::wstring> combo_elements{ L"Mercury", L"Venus", L"Earth", L"Mars", L"Jupiter", L"Saturn", L"Uranus", L"NOT_Neptune" };
	b.add(combo_elements);

	CheckBox aa(win.getHWND(), L"Check", 0.5f, 0.3f);
	CheckBox aac(win.getHWND(), L"Check2", 0.5f, 0.4f);
	
	RadioButton aaa(win.getHWND(), L"Radio", 0.4f, 0.6f, 0.1f, 0.1f,  RESIZABLE);
	RadioButton aaav(win.getHWND(), L"Radio2", 0.4f, 0.7f, 0.1f, 0.1f, RESIZABLE);
	

	Window::wait_msg_proc();

	return 0;
}
