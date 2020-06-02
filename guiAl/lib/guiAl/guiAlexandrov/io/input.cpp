
#include "io/input.h"


namespace gui
{

	// ======================= Mouse ============================

	bool Mouse::pressed(int code)
	{
		return __pressed[code - 512] == 1;
	}

	int Mouse::was_pressed(int code)
	{
		bool temp = __was_pressed[code];
		__was_pressed[code - 512] = false;
		return temp;
	}

	// globals
	int Mouse::__pressed[16];
	bool Mouse::__was_pressed[16];
	float Mouse::pos_x = 0.0f;
	float Mouse::pos_y = 0.0f;





	// ========================= keyboard ==============================

	bool Input::any_key_pressed()
	{
		bool temp = __pressed_any;
		__pressed_any = false;
		return temp;
	}


	bool Input::pressed(int key_code)
	{
		return __pressed[key_code] > 0;
	}


	bool Input::clicked(int key_code)
	{
		return __pressed[key_code] == 1;
	}

	// was pressed from last call
	bool Input::was_pressed(int key_code)
	{
		bool temp = __was_pressed[key_code];
		__was_pressed[key_code] = false;
		return  temp;
	}

	void Input::keys_buffer_clear()
	{
		memset(__pressed, 0, sizeof(__pressed));
	}


	// =========== system input =============

	bool Input::syskey_pressed(int code)
	{
		bool temp = false;
		if (code == VK_ALT)
			temp = vk_alt, vk_alt = false;

		//if (code == VK_F10)
		//	temp = vk_f10, vk_f10 = false;

		return temp;
	}

	// globals
	int Input::__pressed[256];
	bool Input::__pressed_any = false;
	bool Input::__was_pressed[256];

	bool Input::vk_alt = false;
	bool Input::vk_f10 = false;

}