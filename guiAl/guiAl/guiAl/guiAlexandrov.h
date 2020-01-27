#pragma once

#include <Windows.h>

#include <cstdint>
#include <cstdio>

#include <vector>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// globals
HINSTANCE hInst;

void al_init(HINSTANCE hInstance)
{
	hInst = hInstance;
}

// unity build
#include "thread_pool.cpp"
thread_pool workers(2);

// laoyt
#include "canvas.cpp"
#include "window_components.cpp"
#include "window.cpp"
#include "image.cpp"
#include "draw.cpp"
#include "input.cpp"
#include "timer.cpp"
