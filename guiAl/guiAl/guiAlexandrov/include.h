#pragma once

#define safe_release(ptr) (delete *ptr, *ptr = nullptr)
#define safe_releaseArr(ptr) (delete[] *ptr, *ptr = nullptr)


#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>

#include <cstdint>
#include <cstdio>

#include <vector>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include "stb_image.h"

// globals
HINSTANCE hInst;

void al_init(HINSTANCE hInstance)
{
	hInst = hInstance;
}

#ifndef MAX_THREADS
#define MAX_THREADS 4
#endif

// unity build
#include "thread_pool.cpp"
thread_pool workers(MAX_THREADS);

// laoyt
#include "canvas.cpp"
#include "window.cpp"
#include "image.cpp"
#include "draw.cpp"
#include "input.cpp"
#include "timer.cpp"
