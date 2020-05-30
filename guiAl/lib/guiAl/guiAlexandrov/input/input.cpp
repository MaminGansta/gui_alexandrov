
#include "input.h"

// ======================= Mouse ============================


bool Mouse::pressed(int code)
{
	return buttons[code - 512] == 1;
}


bool Mouse::clicked(int code)
{
	return buttons[code - 512] == 1;
}


int Mouse::was_pressed(int code)
{
	bool temp = key_was_pressed[code];
	key_was_pressed[code - 512] = false;
	return temp;
}

// globals
int Mouse::buttons[16];
bool Mouse::key_was_pressed[16];
float Mouse::pos_x = 0.0f;
float Mouse::pos_y = 0.0f;





// ========================= keyboard ==============================


bool Input::any_key_pressed()
{
	bool temp = pressed_any;
	pressed_any = false;
	return temp;
}


bool Input::pressed(int key_code)
{
	return keys[key_code] > 0;
}


bool Input::clicked(int key_code)
{
	return keys[key_code] == 1;
}

	// was pressed from last call
bool Input::was_pressed(int key_code)
{
	bool temp = key_was_pressed[key_code];
	key_was_pressed[key_code] = false;
	return  temp;
}

void Input::keys_buffer_clear()
{
	memset(keys, 0, sizeof(keys));
}


// =========== char input ==============

// was pressed from last call
bool Input::char_pressed(wchar_t char_code)
{
	bool temp = char_buffer[char_code];
	char_buffer[char_code] = false;
	return temp;
}

void Input::char_buffer_clear()
{
	memset(char_buffer, 0, sizeof(char_buffer));
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
int Input::keys[256];
bool Input::pressed_any = false;
bool Input::char_buffer[512];
bool Input::key_was_pressed[256];

bool Input::vk_alt = false;
bool Input::vk_f10 = false;
