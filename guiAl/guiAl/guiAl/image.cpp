

struct Image
{
	int height, width;
	Color* data = NULL;
	bool invalid = false;

	Image(const char* filename)
	{
		int chanels;
		uint8_t* raw = stbi_load(filename, &width, &height, &chanels, 0);

		if (raw == NULL)
		{
			invalid = true;
			return;
		}
		
		data = new Color[width * height];
		
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int pos = (y * width + x) * chanels;
				data[(height - y - 1) * width + x] = Color(raw[pos], raw[pos + 1], raw[pos +2]);
			}
		}

		stbi_image_free(raw);
	}

	Image(int width, int height) : width(width), height(height)
	{
		data = new Color[width * height];
		invalid = false;
	}

	Image(Image& copy)
	{
		height = copy.height;
		width = copy.width;
		data = new Color[height * width];
		memmove(data, copy.data, sizeof(Color) * height * width);
	}

	Image(Image&& other)
	{
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		other.invalid = true;
	}

	Image& operator = (Image&& other)
	{
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		other.invalid = true;
		return *this;
	}


	Color& get_pixel(int x, int y)
	{
		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}
	
	Color& operator [] (int idx)
	{
		assert((uint32_t)idx < width * height);
		return data[idx];
	}

	Color& get_pixel_scaled(int x, int y, int screen_w, int screen_h)
	{
		y = y * height / screen_h;
		x = x * width / screen_w;
	
		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	Color get_pixel_scaled_back_white(int x, int y, int screen_w, int screen_h)
	{
		y = y * height / screen_h;
		x = x * width / screen_w;

		assert(((uint32_t)y < height) | ((uint32_t)x < width));

		Color color = data[y * width + x];
		uint8_t b_w = 255 * (color.r + color.g + color.b) / (255 * 3);
		return Color(b_w);
	}

	~Image() { delete[] data; }
};


void draw_image(Canvas& surface, Image& image,
				int pos_x, int pos_y, int width, int height)
{
	width = min(width, surface.width - pos_x);
	height= min(height, surface.height- pos_y);

	std::future<void> res[8];

	for (int i = 0; i < workers.size; i++)
	{
		int from_x = pos_x + i * width / workers.size;
		int to_x = pos_x + (i + 1) * width / workers.size;

		res[i] = workers.add_task([from_x, to_x, pos_y, &height, &width, &surface, &image]()
		{
			for (int y = pos_y; y < height; y++)
				for (int x = from_x; x < to_x; x++)
				{
					surface.memory[y * surface.width + x] = image.get_pixel_scaled(x, y, width, height);
				}
		});
	}

	for (int i = 0; i < workers.size; i++)
		res[i].get();
}