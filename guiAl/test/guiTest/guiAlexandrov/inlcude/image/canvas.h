#pragma once

#include <Windows.h>
#include <cassert>
#include "color.h"


struct Canvas
{
	int height = 0, width = 0;
	int whole_size = 0;
	int capacity = 0;
	Color* data = nullptr;

	BITMAPINFO bitmap_info;

	Canvas();

	~Canvas();

	void resize(HWND hwnd);

	Color& operator [] (int inx);

	void reserve(int capacity);
};