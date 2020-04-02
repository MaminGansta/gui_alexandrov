#define MAX_THREADS 4
#include "guiAlexandrov/include.h"


template <typename Image_type>
struct My_window : Window
{
	Image_type bg;

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
				case WM_PAINT:
				{
					PAINTSTRUCT plug;
					BeginPaint(hwnd, &plug);
					
					draw_image(window->canvas, window->bg, 0.0f, 0.0f, 1.0f, 1.0f);
					window->render_canvas();

					EndPaint(hwnd, &plug);
				}return 0;

				case WM_CLOSE:
					PostQuitMessage(0);

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

	//My_window<Image> window;

	fImage fbg(L"back.png");
	Image bg = fbg;
	
	Gaussian_filter<Image, 1> gf;
	Gaussian_filter<fImage, 1> fgf;

	float start = get_time();
	//bg = RGB2YCbCr(bg);
	//bg = gauss_filter(bg);
	//bg = sharp_filter(bg);
	//bg = sobel(bg);
	//bg = sobel_avg(bg);

	//bg = auto_contrast(bg);
	//bg = hist_alignment(bg);
	//bg = gray_world(bg);

	bg = gf.apply_async(bg);

	output(L"\n%f\n", get_time() - start);
	Create_Image_window(bg);

	
	start = get_time();
	//fbg = gf.apply(fbg);
	//fbg = RGB2YCbCr(fbg);
	//fbg = gauss_filter(fbg);
	//fbg = sharp_filter(fbg);
	//fbg = sobel(fbg);
	//fbg = sobel_avg(fbg);

	  
	//fbg = auto_contrast(fbg);
	//fbg = hist_alignment(fbg);
	//fbg = gray_world(fbg);

	fbg = fgf.apply_async(fbg);

	output(L"\n%f\n", get_time() - start);
	Create_Image_window(fbg);



	Window::wait_msg_proc();

	return 0;
}
