#pragma once

#include <Windows.h>
#include <string>

#include "Image_base.h"

namespace gui
{

	// ============== from text ======================

#define wtoi(str) ( str ? _wtoi(str) : 0)
#define wtof(str) ( str ? _wtof(str) : 0.0f)


// ================== font =======================

// Defult font for all compontnts
	extern HFONT DEF_FONT;


	HFONT get_def_font(int size = 16);

	void set_font_size(HWND hwnd, int size);

	void set_font(HWND hwnd, HFONT font);


	// =============== render Text ====================

	HBITMAP CreateHBITMAPfromByteArray(HDC hdc, int nImageWidth, int nImageHeight, void* pImageData);

	int WriteTextOnByteArray(int nImageWidth, int nImageHeight, int x, int y, void* pImageData, HFONT hFont, const wchar_t* szText, COLORREF textColor);


	// Font will be reliesed
	template <typename T>
	void render_text(Image_base<T>& surface, float fx, float fy, const std::wstring& text, const Color& color = Color(255), HFONT hFont = gui::get_def_font())
	{
		int x = surface.width * fx;
		int y = surface.height * (1.0f - fy);
		WriteTextOnByteArray(surface.width, surface.height, x, y, surface.data, hFont, text.c_str(), RGB(color.r, color.g, color.b));
		DeleteObject(hFont);
	}


	// Font will be reliesed
	template <typename T>
	void render_text_ml(Image_base<T>& surface, float fx, float fy, const std::wstring& text, const Color& color = Color(255), float y_pad_aspect = 1.2f, HFONT hFont = gui::get_def_font())
	{
		int x = surface.width * fx;
		int y = surface.height * (1.0f - fy);

		int priv_pos = 0, pos = 0, size = 0;
		int temp = 0, offset = 0;
		wchar_t* line = (wchar_t*)_malloca(sizeof(wchar_t) * text.size());

		while (pos < text.size())
		{
			temp = text.find(L'\n', pos);
			pos = temp == -1 ? text.size() : temp;
			size = pos - priv_pos;

			memmove(line, text.c_str() + priv_pos, size * sizeof(wchar_t));
			line[size] = L'\0';

			offset += WriteTextOnByteArray(surface.width, surface.height, x, y + offset, surface.data, hFont, line, RGB(color.r, color.g, color.b)) * y_pad_aspect;
			priv_pos = ++pos;
		}
		DeleteObject(hFont);
	}

}