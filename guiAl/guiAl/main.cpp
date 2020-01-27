#include "guiAl/guiAlexandrov.h"

bool running = true;
Image bg("back.png");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	al_init(hInstance);

	Timer timer(true);
	
	Window win(L"window", 800, 600, DEF_STYLE, NULL, NULL, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		args arg = arguments.get(hwnd);
		if (arg[0] == NULL) return DefWindowProc(hwnd, msg, wParam, lParam);

		Window* window = (Window*)arg[0];

		// arg[1] pointer to your arg

		if (msg == WM_CLOSE) running = false;
		if (msg == WM_SIZE)
		{
			window->canvas.resize(hwnd);
			//window->resize_chileds();
		}
		if (msg == WM_PAINT)
		{
			PAINTSTRUCT plug;
			BeginPaint(hwnd, &plug);
			draw_image(window->canvas, bg, 0, 0, window->canvas.width, window->canvas.height);
			window->render_canvas();
			EndPaint(hwnd, &plug);
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	});

	Button btn(L"button", win.getHWND(), 10);

	while (running)
	{
		Window::default_msg_proc();
		timer.update();
		//char log[16];
		//sprintf_s(log, "%d\n", timer.FPS);
		//OutputDebugStringA(log);
	}

	return 0;
}
