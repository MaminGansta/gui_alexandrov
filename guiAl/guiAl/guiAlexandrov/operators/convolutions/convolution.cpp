
#define my_abs(value) \
{\
	uint32_t temp = value >> 31;\
	value ^= temp;				\
	value += temp & 1;			\
	return value;				\
}


// ============== Kernal class ===================

template <size_t size>
struct Kernal
{
	float kernal[size][size];


	Kernal(std::initializer_list<float> coefs)
	{
		assert(size * size == coefs.size());
		auto coef_ptr = coefs.begin();
		float total = 0;

		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				total += *coef_ptr;
				kernal[i][j] = *coef_ptr++;
			}
		}
	}

	Kernal() = default;

	fImage apply(const fImage& original)
	{
		// get RVO
		fImage res;
		res.resize(original.width, original.height);

		// main area
		int pad = size / 2;

		int x0 = pad;
		int y0 = pad;
		int width = original.width - pad;
		int height = original.height - pad;

		for (int y = y0; y < height; y++)
		{
			for (int x = x0; x < width; x++)
			{
				// rgb(rgba) is bgr(bgra) in windows (i dnk why)
				__m128 bgr = { 0.0f, 0.0f, 0.0f, 0.0f };
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						__m128 pixel = _mm_load_ps(&original[(y - pad + i) * original.width + x - pad + j].b);
						__m128 coef = _mm_set_ps1(kernal[i][j]);
						bgr = _mm_add_ps(bgr, _mm_mul_ps(pixel, coef));
					}
				}
				res[y * res.width + x].b = max(min(bgr.m128_f32[0], 1.0f), 0.0f);
				res[y * res.width + x].g = max(min(bgr.m128_f32[1], 1.0f), 0.0f);
				res[y * res.width + x].r = max(min(bgr.m128_f32[2], 1.0f), 0.0f);
			}
		}
		

		// edges
		for (int i = 0; i < 2; i++)
		{
			for (int y = i * (original.height - pad); y < (1 - i) * pad + i * (original.height); y++)
			{
				for (int x = 0; x < original.width; x++)
				{
					__m128 bgr = { 0.0f, 0.0f, 0.0f, 0.0f };
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							__m128 pixel = _mm_load_ps(&original[core_y * original.width + core_x].b);
							__m128 coef = _mm_set_ps1(kernal[i][j]);
							bgr = _mm_add_ps(bgr, _mm_mul_ps(pixel, coef));
						}
					}
					res[y * res.width + x].b = max(min(bgr.m128_f32[0], 1.0f), 0.0f);
					res[y * res.width + x].g = max(min(bgr.m128_f32[1], 1.0f), 0.0f);
					res[y * res.width + x].r = max(min(bgr.m128_f32[2], 1.0f), 0.0f);
				}
			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int y = pad; y < original.height - pad; y++)
			{
				for (int x = i * (original.width - pad); x < (1 - i) * pad + i * original.width; x++)
				{
					__m128 bgr = { 0.0f, 0.0f, 0.0f, 0.0f };
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							__m128 pixel = _mm_load_ps(&original[core_y * original.width + core_x].b);
							__m128 coef = _mm_set_ps1(kernal[i][j]);
							bgr = _mm_add_ps(bgr, _mm_mul_ps(pixel, coef));
						}
					}
					res[y * res.width + x].b = max(min(bgr.m128_f32[0], 1.0f), 0.0f);
					res[y * res.width + x].g = max(min(bgr.m128_f32[1], 1.0f), 0.0f);
					res[y * res.width + x].r = max(min(bgr.m128_f32[2], 1.0f), 0.0f);
				}
			}
		}

		return res;
	}


	// only for brightnes chanel
	fImage apply_YCbCr(const fImage& original)
	{
		fImage res;
		res.resize(original.width, original.height);

		// main area
		int pad = size / 2;

		int x0 = pad;
		int y0 = pad;
		int width = original.width - pad;
		int height = original.height - pad;

		for (int y = y0; y < height; y++)
		{
			for (int x = x0; x < width; x++)
			{
				float brightnes = 0.0f;
				for (int i = 0; i < size; i++)
					for (int j = 0; j < size; j++)
						brightnes += original[(y - pad + i) * original.width + x - pad + j].Y * kernal[i][j];
				
				res[y * res.width + x].Y = color_clipf(brightnes);
				res[y * res.width + x].U = 0.0f;
				res[y * res.width + x].V = 0.0f;
			}
		}


		// edges
		for (int i = 0; i < 2; i++)
		{
			for (int y = i * (original.height - pad); y < (1 - i) * pad + i * (original.height); y++)
			{
				for (int x = 0; x < original.width; x++)
				{
					float brightnes = 0.0f;
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							brightnes += original[core_y * original.width + core_x].Y * kernal[i][j];

						}
					}
					res[y * res.width + x].Y = color_clipf(brightnes);
					res[y * res.width + x].U = 0.0f;
					res[y * res.width + x].V = 0.0f;
				}
			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int y = pad; y < original.height - pad; y++)
			{
				for (int x = i * (original.width - pad); x < (1 - i) * pad + i * original.width; x++)
				{
					float brightnes = 0.0f;
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;
							
							brightnes += original[core_y * original.width + core_x].Y * kernal[i][j];
						}
					}
					res[y * res.width + x].Y = color_clipf(brightnes);
					res[y * res.width + x].U = 0.0f;
					res[y * res.width + x].V = 0.0f;
				}
			}
		}

		return res;
	}


	float* operator [] (int i) { return kernal[i]; }
};


// ====================== Gaussian filter ===========================

constexpr int mat_size(size_t sigma)
{
	return sigma * 6 + 1;
}

template <size_t sigma>
constexpr int gauss_construcor(float kernal[mat_size(sigma)][mat_size(sigma)])
{
	size_t size = mat_size(sigma);
	float A, a, c;
	float sigma_quad = sigma * sigma;
	A = 1.0f / (2.0f * PI * sigma_quad);
	a = c = 1.0f / (2.0f * sigma_quad);


	float total = 0;
	int pad = size / 2;
	for (int y = -pad; y <= pad; y++)
	{
		for (int x = -pad; x <= pad; x++)
		{
			float temp = A * expf(-(a * (x * x) + 2 * (abs(x) * abs(y)) + c * (y * y)));
			kernal[y + pad][x + pad] = temp;
			total += temp;
		}
	}

	// normalize coefs
	total = 1.0f / total;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			kernal[i][j] *= total;
		}
	}

	return mat_size(sigma);
}


template <size_t sigma>
struct Gaussian_filter :  Kernal<mat_size(sigma)>
{
	using Kernal<mat_size(sigma)>::kernal;

	// compile time constructor
	int size = gauss_construcor<sigma>(kernal);

	Gaussian_filter() = default;
};


// =================== Sharp filter ====================

struct Sharp_filter : public Kernal<3>
{

	Sharp_filter() : Kernal({ -0.1f, -0.2f, -0.1f ,-0.2f, 2.2f, -0.2f ,-0.1f, -0.2f, -0.1f }){}
};


// ================ Sobel's operator ====================

struct Sobel_y : public Kernal<3>
{
	Sobel_y() : Kernal({ 1, 0 , -1, 2, 0, -2, 1, 0 , -1 }) {}
};

struct Sobel_x : public Kernal<3>
{
	Sobel_x() : Kernal({ 1, 2 , 1, 0, 0, 0, -1, -2 , -1 }) {}
};


fImage sobel(const fImage& origin)
{
	fImage res(origin.width, origin.height);
	Sobel_y sobel_y;
	Sobel_x sobel_x;

	fImage sob_x = sobel_y.apply_YCbCr(RGB2YCbCr(origin));
	fImage sob_y = sobel_x.apply_YCbCr(RGB2YCbCr(origin));

	for (int i = 0; i < origin.width * origin.height; i++)
		res[i] = (sob_x[i] + sob_y[i]) / 2;

	return YCbCr2RGB(res);
}
