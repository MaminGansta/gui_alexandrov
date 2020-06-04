#pragma once

#include <string.h>


namespace gui
{

	// ======================= Mouse ============================

	struct Mouse
	{
		static int __pressed[16];
		static bool __was_pressed[16];
		static float pos_x, pos_y;

		static bool pressed(int code);

		static int was_pressed(int code);
	};






	// ========================= keyboard ==============================

	struct Input
	{
		static bool __pressed_any;

		// ============== key input =================
		static int __pressed[256];

		// if key was pressed from last call
		static bool __was_pressed[256];

		
		// from last call
		static bool any_key_pressed();


		static bool pressed(int key_code);

		static bool clicked(int key_code);

		// was pressed from last call
		static bool was_pressed(int key_code);

		// set all values false
		static void keys_buffer_clear();


		// =========== system input =============
#define VK_ALT 0

		static bool vk_alt;
		static bool vk_f10;

		static bool syskey_pressed(int code);
	};

}