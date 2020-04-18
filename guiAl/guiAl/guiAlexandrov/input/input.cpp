
// ======================= Mouse ============================

enum Mouse_buttons
{
	LBUTTON,
	RBUTTON,

	MOUSE_BUTTONS_COUNT
};


struct Mouse_Input
{
	int pos_x, pos_y;
	
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
	static bool any_key_pressed()
	{
		bool temp = pressed_any;
		pressed_any = false;
		return temp;
	}

	static bool pressed(int key_code)
	{
		return keys[key_code] > 1;
	}

	static bool clicked(int key_code)
	{
		return keys[key_code] == 1;
	}

	// was pressed from last call
	static bool was_pressed(int key_code)
	{
		bool temp = key_was_pressed[key_code];
		key_was_pressed[key_code] = false;
		return  temp;
	}


	// =========== char input ==============
	static bool char_buffer[512];

	// was pressed from last call
	static bool char_pressed(wchar_t char_code)
	{
		bool temp = char_buffer[char_code];
		char_buffer[char_code] = false;
		return temp;
	}

	static void char_buffer_clear()
	{
		memset(char_buffer, 0, sizeof(char_buffer));
	}

	// =========== system input =============
#define VK_ALT 0
	static bool vk_alt;
	static bool vk_f10;

	static bool syskey_pressed(int code)
	{
		bool temp = false;
		if (code == VK_ALT)
			temp = vk_alt, vk_alt = false;

		if (code == VK_F10)
			temp = vk_f10, vk_f10 = false;

		return temp;
	}

};

int Input::keys[256];
bool Input::pressed_any = false;
bool Input::char_buffer[512];
bool Input::key_was_pressed[256];

bool Input::vk_alt = false;
bool Input::vk_f10 = false;



