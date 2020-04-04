#include <emmintrin.h>

static inline __m128i muly(const __m128i& a, const __m128i& b)
{
	__m128i tmp1 = _mm_mul_epu32(a, b); /* mul 2,0*/
	__m128i tmp2 = _mm_mul_epu32(_mm_srli_si128(a, 4), _mm_srli_si128(b, 4)); /* mul 3,1 */
	return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE(0, 0, 2, 0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE(0, 0, 2, 0))); /* shuffle results to [63..0] and pack */
}

// ============== Kernal class ===================
template  <typename Image_type, size_t size>
struct Kernal;


// ========================== fImage represintation ===============================
template <size_t size>
struct Kernal<fImage, size>
{
	float kernal[size][size];
	
	Kernal() = default;
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
	void Kernal_init(float kernal[size][size])
	{
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				this->kernal[i][j] = kernal[i][j];
	}

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



	fImage apply_async(const fImage& original)
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

		std::future<void> threads[MAX_THREADS];

		int from, to;
		for (int i = 0; i < workers.size; i++)
		{
			from = height * i / workers.size;
			to = height * (i + 1) / workers.size;

			if (i == 0)
			{
				from += pad;
				to += pad;
			}

			threads[i] = workers.add_task([=, &original, &res]()
				{
					for (int y = from; y < to; y++)
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
				});
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

		for (int i = 0; i < workers.size; i++)
			threads[i].get();

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
				
				res[y * res.width + x].Y = chanel_clip(brightnes);
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
					res[y * res.width + x].Y = chanel_clip(brightnes);
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
					res[y * res.width + x].Y = chanel_clip(brightnes);
					res[y * res.width + x].U = 0.0f;
					res[y * res.width + x].V = 0.0f;
				}
			}
		}

		return res;
	}


	float* operator [] (int i) { return kernal[i]; }
};


// ========================= Image representation ===================================
template <size_t size>
struct Kernal<Image, size>
{
	int coef = 10000;
	int kernal[size][size];
	
	Kernal() = default;
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
				kernal[i][j] = *coef_ptr++ * 10000.0f;
			}
		}
	}
	void Kernal_init(float kernal[size][size])
	{
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				this->kernal[i][j] = kernal[i][j] * 10000.0f;
	}


	Image apply(const Image& original)
	{
		// get RVO
		Image res;
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
				int r = 0, g = 0, b = 0;
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						const Color& pixel = original[(y - pad + i) * original.width + x - pad + j];
						r += pixel.r * kernal[i][j];
						g += pixel.g * kernal[i][j];
						b += pixel.b * kernal[i][j];
					}
				}
				res[y * res.width + x].r = chanel_clip(r / coef);
				res[y * res.width + x].g = chanel_clip(g / coef);
				res[y * res.width + x].b = chanel_clip(b / coef);
			}
		}


		// edges
		for (int i = 0; i < 2; i++)
		{
			for (int y = i * (original.height - pad); y < (1 - i) * pad + i * (original.height); y++)
			{
				for (int x = 0; x < original.width; x++)
				{
					int r = 0, g = 0, b = 0;
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							const Color& pixel = original[core_y * original.width + core_x];

							r += pixel.r * kernal[i][j];
							g += pixel.g * kernal[i][j];
							b += pixel.b * kernal[i][j];
						}
					}
					res[y * res.width + x].r = chanel_clip(r / coef);
					res[y * res.width + x].g = chanel_clip(g / coef);
					res[y * res.width + x].b = chanel_clip(b / coef);
				}
			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int y = pad; y < original.height - pad; y++)
			{
				for (int x = i * (original.width - pad); x < (1 - i) * pad + i * original.width; x++)
				{
					int r = 0, g = 0, b = 0;
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							const Color& pixel = original[core_y * original.width + core_x];
							r += pixel.r * kernal[i][j];
							g += pixel.g * kernal[i][j];
							b += pixel.b * kernal[i][j];
						}
					}
					res[y * res.width + x].r = chanel_clip(r / coef);
					res[y * res.width + x].g = chanel_clip(g / coef);
					res[y * res.width + x].b = chanel_clip(b / coef);
				}
			}
		}

		return res;
	}


	Image apply_async(const Image& original)
	{
		// get RVO
		Image res;
		res.resize(original.width, original.height);

		// main area
		int pad = size / 2;

		int x0 = pad;
		int y0 = pad;
		int width = original.width - pad;
		int height = original.height - pad;

		std::future<void> threads[MAX_THREADS];

		int from, to;
		for (int i = 0; i < workers.size; i++)
		{
			from = height * i / workers.size;
			to = height * (i + 1) / workers.size;
			
			if (i == 0)
			{
				from += pad;
				to += pad;
			}

			threads[i] = workers.add_task([=, &original, &res]()
				{
					for (int y = from; y < to; y++)
					{
						for (int x = x0; x < width; x++)
						{
							int r = 0, g = 0, b = 0;
							for (int i = 0; i < size; i++)
							{
								for (int j = 0; j < size; j++)
								{
									const Color& pixel = original[(y - pad + i) * original.width + x - pad + j];
									r += pixel.r * kernal[i][j];
									g += pixel.g * kernal[i][j];
									b += pixel.b * kernal[i][j];
								}
							}
							res[y * res.width + x].r = chanel_clip(r / coef);
							res[y * res.width + x].g = chanel_clip(g / coef);
							res[y * res.width + x].b = chanel_clip(b / coef);
						}
					}
				});
		}

		// edges
		for (int i = 0; i < 2; i++)
		{
			for (int y = i * (original.height - pad); y < (1 - i) * pad + i * (original.height); y++)
			{
				for (int x = 0; x < original.width; x++)
				{
					int r = 0, g = 0, b = 0;
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							const Color& pixel = original[core_y * original.width + core_x];

							r += pixel.r * kernal[i][j];
							g += pixel.g * kernal[i][j];
							b += pixel.b * kernal[i][j];
						}
					}
					res[y * res.width + x].r = chanel_clip(r / coef);
					res[y * res.width + x].g = chanel_clip(g / coef);
					res[y * res.width + x].b = chanel_clip(b / coef);
				}
			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int y = pad; y < original.height - pad; y++)
			{
				for (int x = i * (original.width - pad); x < (1 - i) * pad + i * original.width; x++)
				{
					int r = 0, g = 0, b = 0;
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							const Color& pixel = original[core_y * original.width + core_x];
							r += pixel.r * kernal[i][j];
							g += pixel.g * kernal[i][j];
							b += pixel.b * kernal[i][j];
						}
					}
					res[y * res.width + x].r = chanel_clip(r / coef);
					res[y * res.width + x].g = chanel_clip(g / coef);
					res[y * res.width + x].b = chanel_clip(b / coef);
				}
			}
		}

		for (int i = 0; i < workers.size; i++)
			threads[i].get();

		return res;
	}


	// only for brightnes chanel
	Image apply_YCbCr(const Image& original)
	{
		Image res;
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
				int brightnes = 0;
				for (int i = 0; i < size; i++)
					for (int j = 0; j < size; j++)
						brightnes += original[(y - pad + i) * original.width + x - pad + j].Y * kernal[i][j];
	
				res[y * res.width + x].Y = chanel_clip(brightnes / coef);
				res[y * res.width + x].U = 0;
				res[y * res.width + x].V = 0;
			}
		}
	
	
		// edges
		for (int i = 0; i < 2; i++)
		{
			for (int y = i * (original.height - pad); y < (1 - i) * pad + i * (original.height); y++)
			{
				for (int x = 0; x < original.width; x++)
				{
					int brightnes = 0;
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
					res[y * res.width + x].Y = chanel_clip(brightnes / coef);
					res[y * res.width + x].U = 0;
					res[y * res.width + x].V = 0;
				}
			}
		}
	
		for (int i = 0; i < 2; i++)
		{
			for (int y = pad; y < original.height - pad; y++)
			{
				for (int x = i * (original.width - pad); x < (1 - i) * pad + i * original.width; x++)
				{
					int brightnes = 0;
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
					res[y * res.width + x].Y = chanel_clip(brightnes / coef);
					res[y * res.width + x].U = 0;
					res[y * res.width + x].V = 0;
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

template <typename Image_type, size_t sigma = 1>
struct Gaussian_filter :  Kernal<Image_type, mat_size(sigma)>
{
	using Kernal<Image_type, mat_size(sigma)>::Kernal_init;

	Gaussian_filter()
	{
		size_t size = mat_size(sigma);
		float kernal[mat_size(sigma)][mat_size(sigma)];
		
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

		
		Kernal_init(kernal);
	}
};



template <typename Image_type>
Image_type gauss_filter(const Image_type& image)
{
	Gaussian_filter<Image_type, 1> filter;
	return filter.apply(image);
}



// =================== Sharp filter ====================
template <typename Image_type, size_t type = 1>
struct Sharp_filter;

template <typename Image_type>
struct Sharp_filter<Image_type, 1>: public Kernal<Image_type, 3>
{
	Sharp_filter() : Kernal<Image_type, 3>({ -0.1f, -0.2f, -0.1f ,-0.2f, 2.2f, -0.2f ,-0.1f, -0.2f, -0.1f }){}
};

template <typename Image_type>
struct Sharp_filter<Image_type, 2> : public Kernal<Image_type, 3>
{
	Sharp_filter() : Kernal<Image_type, 3>({ -0.25f, -0.25f, -0.25f ,-0.25f, 3.0f, -0.25f ,-0.25f, -0.25f, -0.25f }) {}
};

template <typename Image_type>
struct Sharp_filter<Image_type, 3> : public Kernal<Image_type, 3>
{
	Sharp_filter() : Kernal<Image_type, 3>({ -0.25f, -0.5f, -0.25f ,-0.5f, 4.0f, -0.5f ,-0.25f, -0.5f, -0.25f }) {}
};


template <typename Image_type>
struct Sharp_filter<Image_type, 4> : public Kernal<Image_type, 3>
{
	Sharp_filter() : Kernal<Image_type, 3>({ -0.3f, -0.5f, -0.3f ,-0.5f, 4.2f, -0.5f ,-0.3f, -0.5f, -0.3f }) {}
};



template <typename Image_type>
struct Sharp_filter<Image_type, 5> : public Kernal<Image_type, 3>
{
	Sharp_filter() : Kernal<Image_type, 3>({ -1.0f, -1.0f, -1.0f ,-1.0f, 9.0f, -1.0f ,-1.0f, -1.0f, -1.0f }) {}
};




template <typename Image_type>
Image_type sharp_filter(const Image_type& image)
{
	Sharp_filter<Image_type> sf;
	return sf.apply(image);
}




// ================ Sobel operator ====================
template <typename Image_type>
struct Sobel_y : public Kernal<Image_type, 3>
{
	Sobel_y() : Kernal<Image_type, 3>({ 1, 0 , -1, 2, 0, -2, 1, 0 , -1 }) {}
};


template <typename Image_type>
struct Sobel_x : public Kernal<Image_type, 3>
{
	Sobel_x() : Kernal<Image_type, 3>({ 1, 2 , 1, 0, 0, 0, -1, -2 , -1 }) {}
};

template <typename Image_type>
Image_type sobel(const Image_type& origin)
{
	Image_type res(origin.width, origin.height);
	Sobel_y<Image_type> sobel_y;
	Sobel_x<Image_type> sobel_x;

	Image_type sob_x = sobel_y.apply_YCbCr(RGB2YCbCr(origin));
	Image_type sob_y = sobel_x.apply_YCbCr(RGB2YCbCr(origin));

	for (int i = 0; i < origin.width * origin.height; i++)
		res[i] = (sob_x[i] + sob_y[i]);

	return YCbCr2RGB(res);
}

template <typename Image_type>
Image_type sobel_avg(const Image_type& origin)
{
	Image_type res(origin.width, origin.height);
	Sobel_y<Image_type> sobel_y;
	Sobel_x<Image_type> sobel_x;

	Image_type sob_x = sobel_y.apply_YCbCr(RGB2YCbCr(origin));
	Image_type sob_y = sobel_x.apply_YCbCr(RGB2YCbCr(origin));

	for (int i = 0; i < origin.width * origin.height; i++)
		res[i] = (sob_x[i] / 2 + sob_y[i] / 2);

	return YCbCr2RGB(res);
}


// =============== Box filter =======================
template <typename Image_type, size_t size = 3>
struct Box_filter: public Kernal<Image_type, size>
{
	using Kernal<Image_type, size>::Kernal_init;

	Box_filter()
	{
		float kernal[size][size];
		float coef = 1.0f / (size * size);

		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				kernal[i][j] = coef;

		Kernal_init(kernal);
	}
};


template <typename Image_type>
Image_type box_filter(const Image_type& image)
{
	Box_filter<Image_type> bf;
	return bf.apply(image);
}

