#pragma once

#include <Windows.h>
#include <string>

#include "color.h"


namespace gui
{

	// ============== from text ======================

#define wtoi(str) ( str ? _wtoi(str) : 0)
#define wtof(str) ( str ? _wtof(str) : 0.0f)


// ================== font =======================

// Defult font for all compontnts
	extern HFONT DEF_FONT;


	HFONT get_def_font(int size);

	void set_font_size(HWND hwnd, int size);

	void set_font(HWND hwnd, HFONT font);


	// =============== render Text ====================

	HBITMAP CreateHBITMAPfromByteArray(HDC hdc, int nImageWidth, int nImageHeight, void* pImageData);

	int WriteTextOnByteArray(int nImageWidth, int nImageHeight, int x, int y, void* pImageData, HFONT hFont, const wchar_t* szText, COLORREF textColor);



	template <typename Surface_type>
	void render_text(Surface_type& surface, float fx, float fy, const std::wstring& text, const Color& color = Color(), HFONT hFont = DEF_FONT);


	template <typename Surface_type, size_t buffer_size>
	void render_text_ml(Surface_type& surface, float fx, float fy, const std::wstring& text, const Color& color = Color(), HFONT hFont = DEF_FONT);

}