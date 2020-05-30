#pragma once

#include <string.h>


// ======================= Mouse ============================

struct Mouse
{
	static int buttons[16];
	static bool key_was_pressed[16];
	static float pos_x, pos_y;

	static bool pressed(int code);

	static bool clicked(int code);

	static int was_pressed(int code);
};






// ========================= keyboard ==============================

struct Input
{
	static bool pressed_any;

	// ============== key input =================
	static int keys[256];
	// if key was pressed from last call
	static bool key_was_pressed[256];

	// from last call
	static bool any_key_pressed();

	static bool pressed(int key_code);

	static bool clicked(int key_code);

	// was pressed from last call
	static bool was_pressed(int key_code);

	static void keys_buffer_clear();


	// =========== char input ==============
	static bool char_buffer[512];

	// was pressed from last call
	static bool char_pressed(wchar_t char_code);

	static void char_buffer_clear();

	// =========== system input =============
#define VK_ALT 0
	static bool vk_alt;
	static bool vk_f10;

	static bool syskey_pressed(int code);
};




