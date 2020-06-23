#pragma once

#include <emmintrin.h>

#include <stdint.h>
#include <cstdlib>

#include "../image/image.h"
#include "../window/gui_init.h"

#ifndef PI
#define PI 3.14159265359f
#endif


namespace gui
{
	namespace cpu
	{

		// ============== Kernel class ===================
		template  <typename Image_type, size_t size>
		struct Kernel;


		// ========================== fImage represintation ===============================
		template <size_t size>
		struct Kernel<fImage, size>
		{
			float kernel[size][size];

			Kernel() = default;
			Kernel(std::initializer_list<float> coefs)
			{
				assert(size * size == coefs.size());
				auto coef_ptr = coefs.begin();
				float total = 0;

				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						total += *coef_ptr;
						kernel[i][j] = *coef_ptr++;
					}
				}
			}
			void Kernal_init(float Kernel[size][size])
			{
				for (int i = 0; i < size; i++)
					for (int j = 0; j < size; j++)
						this->kernel[i][j] = Kernel[i][j];
			}

			// new convolution from existed
			template <size_t second_size>
			Kernel(const Kernel<fImage, size>& kernelF, const Kernel<fImage, second_size>& kernelS)
			{
				int pad = second_size / 2;
				float total = 0.0f;

				for (int y = 0; y < size; y++)
				{
					for (int x = 0; x < size; x++)
					{
						float new_coef = 0;
						for (int i = 0; i < second_size; i++)
						{
							for (int j = 0; j < second_size; j++)
							{
								int core_y = y + i - pad;
								if (core_y < 0)
									core_y = abs(core_y - 1);

								int core_x = x + j - pad;
								if (core_x < 0)
									core_x = abs(core_x - 1);


								if (core_x >= size) core_x = size - (core_x - size) - 1;
								if (core_y >= size) core_y = size - (core_y - size) - 1;

								new_coef += kernelF.kernel[core_y][core_x] * kernelS.kernel[i][j];
							}
						}
						total += kernel[y][x] = new_coef;
					}
				}

				// normalize coefs
				total = 1.0f / total;
				for (int i = 0; i < size; i++)
					for (int j = 0; j < size; j++)
						kernel[i][j] *= total;

			}


			// ============ Apply convolution to the image ==============
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
								__m128 coef = _mm_set_ps1(kernel[i][j]);
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
									__m128 coef = _mm_set_ps1(kernel[i][j]);
									bgr = _mm_add_ps(bgr, _mm_mul_ps(pixel, coef));
								}
							}
							res[y * res.width + x].b = max(min(bgr.m128_f32[0], 1.0f), 0.0f);
							res[y * res.width + x].g = max(min(bgr.m128_f32[1], 1.0f), 0.0f);
							res[y * res.width + x].r = max(min(bgr.m128_f32[2], 1.0f), 0.0f);
							res[y * res.width + x].a = original[y * res.width + x].a;
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
									__m128 coef = _mm_set_ps1(kernel[i][j]);
									bgr = _mm_add_ps(bgr, _mm_mul_ps(pixel, coef));
								}
							}
							res[y * res.width + x].b = max(min(bgr.m128_f32[0], 1.0f), 0.0f);
							res[y * res.width + x].g = max(min(bgr.m128_f32[1], 1.0f), 0.0f);
							res[y * res.width + x].r = max(min(bgr.m128_f32[2], 1.0f), 0.0f);
							res[y * res.width + x].a = original[y * res.width + x].a;
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

				thread_pool.parallel_for_void(y0, height, [=, &original, &res](int from, int to, int idx)
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
										__m128 coef = _mm_set_ps1(kernel[i][j]);
										bgr = _mm_add_ps(bgr, _mm_mul_ps(pixel, coef));
									}
								}
								res[y * res.width + x].b = max(min(bgr.m128_f32[0], 1.0f), 0.0f);
								res[y * res.width + x].g = max(min(bgr.m128_f32[1], 1.0f), 0.0f);
								res[y * res.width + x].r = max(min(bgr.m128_f32[2], 1.0f), 0.0f);
								res[y * res.width + x].a = original[y * res.width + x].a;
							}
						}
					}
				);
				thread_pool.wait();

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
									__m128 coef = _mm_set_ps1(kernel[i][j]);
									bgr = _mm_add_ps(bgr, _mm_mul_ps(pixel, coef));
								}
							}
							res[y * res.width + x].b = max(min(bgr.m128_f32[0], 1.0f), 0.0f);
							res[y * res.width + x].g = max(min(bgr.m128_f32[1], 1.0f), 0.0f);
							res[y * res.width + x].r = max(min(bgr.m128_f32[2], 1.0f), 0.0f);
							res[y * res.width + x].a = original[y * res.width + x].a;
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
									__m128 coef = _mm_set_ps1(kernel[i][j]);
									bgr = _mm_add_ps(bgr, _mm_mul_ps(pixel, coef));
								}
							}
							res[y * res.width + x].b = max(min(bgr.m128_f32[0], 1.0f), 0.0f);
							res[y * res.width + x].g = max(min(bgr.m128_f32[1], 1.0f), 0.0f);
							res[y * res.width + x].r = max(min(bgr.m128_f32[2], 1.0f), 0.0f);
							res[y * res.width + x].a = original[y * res.width + x].a;
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
								brightnes += original[(y - pad + i) * original.width + x - pad + j].Y * kernel[i][j];

						res[y * res.width + x].Y = chanel_clip(brightnes);
						res[y * res.width + x].U = original[y * res.width + x].U;
						res[y * res.width + x].V = original[y * res.width + x].V;
						res[y * res.width + x].a = original[y * res.width + x].a;
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

									brightnes += original[core_y * original.width + core_x].Y * kernel[i][j];

								}
							}
							res[y * res.width + x].Y = chanel_clip(brightnes);
							res[y * res.width + x].U = original[y * res.width + x].U;
							res[y * res.width + x].V = original[y * res.width + x].V;
							res[y * res.width + x].a = original[y * res.width + x].a;
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

									brightnes += original[core_y * original.width + core_x].Y * Kernel[i][j];
								}
							}
							res[y * res.width + x].Y = chanel_clip(brightnes);
							res[y * res.width + x].U = original[y * res.width + x].U;
							res[y * res.width + x].V = original[y * res.width + x].V;
							res[y * res.width + x].a = original[y * res.width + x].a;
						}
					}
				}

				return res;
			}


			float* operator [] (int i) { return Kernel[i]; }
		};


		// ========================= Image representation ===================================
		template <size_t size>
		struct Kernel<Image, size>
		{
			int coef = 10000;
			int kernel[size][size];

			Kernel() = default;
			Kernel(std::initializer_list<float> coefs)
			{
				assert(size * size == coefs.size());
				auto coef_ptr = coefs.begin();
				float total = 0;

				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						total += *coef_ptr;
						kernel[i][j] = *coef_ptr++ * 10000.0f;
					}
				}
			}
			void Kernal_init(float Kernel[size][size])
			{
				for (int i = 0; i < size; i++)
					for (int j = 0; j < size; j++)
						this->kernel[i][j] = Kernel[i][j] * 10000.0f;
			}


			// new convolution from existed
			template <size_t second_size>
			Kernel(const Kernel<Image, size>& kernelF, const Kernel<Image, second_size>& kernelS)
			{
				int pad = second_size / 2;
				float total = 0.0f;

				for (int y = 0; y < size; y++)
				{
					for (int x = 0; x < size; x++)
					{
						int new_coef = 0;
						for (int i = 0; i < second_size; i++)
						{
							for (int j = 0; j < second_size; j++)
							{
								int core_y = y + i - pad;
								if (core_y < 0)
									core_y = abs(core_y - 1);

								int core_x = x + j - pad;
								if (core_x < 0)
									core_x = abs(core_x - 1);


								if (core_x >= size) core_x = size - (core_x - size) - 1;
								if (core_y >= size) core_y = size - (core_y - size) - 1;

								new_coef += kernelF.kernel[core_y][core_x] * kernelS.kernel[i][j];
							}
						}
						total += Kernel[y][x] = new_coef / coef;
					}
				}

				// normalize coefs
				total = coef / total;
				for (int i = 0; i < size; i++)
					for (int j = 0; j < size; j++)
						kernel[i][j] *= total;

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
								r += pixel.r * kernel[i][j];
								g += pixel.g * kernel[i][j];
								b += pixel.b * kernel[i][j];
							}
						}
						res[y * res.width + x].r = chanel_clip(r / coef);
						res[y * res.width + x].g = chanel_clip(g / coef);
						res[y * res.width + x].b = chanel_clip(b / coef);
						res[y * res.width + x].a = original[y * res.width + x].a;
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

									r += pixel.r * kernel[i][j];
									g += pixel.g * kernel[i][j];
									b += pixel.b * kernel[i][j];
								}
							}
							res[y * res.width + x].r = chanel_clip(r / coef);
							res[y * res.width + x].g = chanel_clip(g / coef);
							res[y * res.width + x].b = chanel_clip(b / coef);
							res[y * res.width + x].a = original[y * res.width + x].a;
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
									r += pixel.r * kernel[i][j];
									g += pixel.g * kernel[i][j];
									b += pixel.b * kernel[i][j];
								}
							}
							res[y * res.width + x].r = chanel_clip(r / coef);
							res[y * res.width + x].g = chanel_clip(g / coef);
							res[y * res.width + x].b = chanel_clip(b / coef);
							res[y * res.width + x].a = original[y * res.width + x].a;
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

				thread_pool.parallel_for_void(y0, height, [=, &original, &res](int from, int to, int idx)
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
										r += pixel.r * kernel[i][j];
										g += pixel.g * kernel[i][j];
										b += pixel.b * kernel[i][j];
									}
								}
								res[y * res.width + x].r = chanel_clip(r / coef);
								res[y * res.width + x].g = chanel_clip(g / coef);
								res[y * res.width + x].b = chanel_clip(b / coef);
								res[y * res.width + x].a = original[y * res.width + x].a;
							}
						}
					}
				);
				thread_pool.wait();

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

									r += pixel.r * kernel[i][j];
									g += pixel.g * kernel[i][j];
									b += pixel.b * kernel[i][j];
								}
							}
							res[y * res.width + x].r = chanel_clip(r / coef);
							res[y * res.width + x].g = chanel_clip(g / coef);
							res[y * res.width + x].b = chanel_clip(b / coef);
							res[y * res.width + x].a = original[y * res.width + x].a;
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
									r += pixel.r * kernel[i][j];
									g += pixel.g * kernel[i][j];
									b += pixel.b * kernel[i][j];
								}
							}
							res[y * res.width + x].r = chanel_clip(r / coef);
							res[y * res.width + x].g = chanel_clip(g / coef);
							res[y * res.width + x].b = chanel_clip(b / coef);
							res[y * res.width + x].a = original[y * res.width + x].a;
						}
					}
				}

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
								brightnes += original[(y - pad + i) * original.width + x - pad + j].Y * kernel[i][j];

						res[y * res.width + x].Y = chanel_clip(brightnes / coef);
						res[y * res.width + x].U = original[y * res.width + x].U;
						res[y * res.width + x].V = original[y * res.width + x].V;
						res[y * res.width + x].a = original[y * res.width + x].a;
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

									brightnes += original[core_y * original.width + core_x].Y * kernel[i][j];

								}
							}
							res[y * res.width + x].Y = chanel_clip(brightnes / coef);
							res[y * res.width + x].U = original[y * res.width + x].U;
							res[y * res.width + x].V = original[y * res.width + x].V;
							res[y * res.width + x].a = original[y * res.width + x].a;
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

									brightnes += original[core_y * original.width + core_x].Y * kernel[i][j];
								}
							}
							res[y * res.width + x].Y = chanel_clip(brightnes / coef);
							res[y * res.width + x].U = original[y * res.width + x].U;
							res[y * res.width + x].V = original[y * res.width + x].V;
							res[y * res.width + x].a = original[y * res.width + x].a;
						}
					}
				}

				return res;
			}

			float* operator [] (int i) { return kernel[i]; }
		};

		// =============== make new Kernel ====================
		template <typename Image_type, size_t sizeF, size_t sizeS>
		Kernel<Image_type, sizeF> new_kernal(const Kernel<Image_type, sizeF>& f, const Kernel<Image_type, sizeS>& s)
		{
			return Kernel<Image_type, sizeF>(f, s);
		}





		//	=================================== FILTERS =====================================


		// ====================== Gaussian filter ===========================

		constexpr int mat_size(size_t sigma)
		{
			return sigma * 6 + 1;
		}

		template <typename Image_type, size_t sigma = 1>
		struct Gaussian_filter : Kernel<Image_type, mat_size(sigma)>
		{
			using Kernel<Image_type, mat_size(sigma)>::Kernal_init;

			Gaussian_filter()
			{
				size_t size = mat_size(sigma);
				float Kernel[mat_size(sigma)][mat_size(sigma)];

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
						Kernel[y + pad][x + pad] = temp;
						total += temp;
					}
				}

				// normalize coefs
				total = 1.0f / total;
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						Kernel[i][j] *= total;
					}
				}


				Kernal_init(Kernel);
			}
		};



		template <typename Image_type, size_t sigma = 1>
		Image_type gauss_filter(const Image_type& image)
		{
			Gaussian_filter<Image_type, sigma> filter;
			return filter.apply(image);
		}



		// =================== Sharp filter ====================

		template <typename Image_type, size_t size = 3, size_t sharp = 2>
		struct Sharp_filter_s : Kernel<Image_type, size>
		{
			using Kernel<Image_type, size>::Kernal_init;

			Sharp_filter_s()
			{
				float Kernel[size][size];
				int center = size / 2;
				float total = 0.0f;
				float sharp_norm = float(sharp) / 100.0f;

				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						if (i == center && j == center) continue;
						total += Kernel[i][j] = -sharp_norm / powf(abs(i - center) + abs(j - center), size / 2);
					}
				}

				Kernel[center][center] = -total + 1;
				Kernal_init(Kernel);
			}

		};

		template <typename Image_type, size_t size = 3, size_t sharp = 2>
		Image_type sharp_filter_s(const Image_type& image)
		{
			Sharp_filter_s<Image_type, size, sharp> sf;
			return sf.apply(image);
		}



		// run time coefs determination
		template <typename Image_type, size_t size = 3>
		struct Sharp_filter : Kernel<Image_type, size>
		{
			using Kernel<Image_type, size>::Kernal_init;

			Sharp_filter(int sharp = 2)
			{
				float Kernel[size][size];
				int center = size / 2;
				float total = 0.0f;
				float sharp_norm = float(sharp) / 100.0f;

				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						if (i == center && j == center) continue;
						total += Kernel[i][j] = -sharp_norm / powf(abs(i - center) + abs(j - center), size / 2);
					}
				}

				Kernel[center][center] = -total + 1;
				Kernal_init(Kernel);
			}

		};

		template <typename Image_type, size_t size = 3>
		Image_type sharp_filter(const Image_type& image, int sharp = 2)
		{
			Sharp_filter<Image_type, size> sf(sharp);
			return sf.apply(image);
		}



		template <typename Image_type, size_t type = 1>
		struct Sharp_filter3x3;

		template <typename Image_type>
		struct Sharp_filter3x3<Image_type, 1> : public Kernel<Image_type, 3>
		{
			Sharp_filter3x3() : Kernel<Image_type, 3>({ -0.1f, -0.2f, -0.1f ,-0.2f, 2.2f, -0.2f ,-0.1f, -0.2f, -0.1f }) {}
		};

		template <typename Image_type>
		struct Sharp_filter3x3<Image_type, 2> : public Kernel<Image_type, 3>
		{
			Sharp_filter3x3() : Kernel<Image_type, 3>({ -0.25f, -0.25f, -0.25f ,-0.25f, 3.0f, -0.25f ,-0.25f, -0.25f, -0.25f }) {}
		};

		template <typename Image_type>
		struct Sharp_filter3x3<Image_type, 3> : public Kernel<Image_type, 3>
		{
			Sharp_filter3x3() : Kernel<Image_type, 3>({ -0.25f, -0.5f, -0.25f ,-0.5f, 4.0f, -0.5f ,-0.25f, -0.5f, -0.25f }) {}
		};


		template <typename Image_type>
		struct Sharp_filter3x3<Image_type, 4> : public Kernel<Image_type, 3>
		{
			Sharp_filter3x3() : Kernel<Image_type, 3>({ -0.3f, -0.5f, -0.3f ,-0.5f, 4.2f, -0.5f ,-0.3f, -0.5f, -0.3f }) {}
		};


		template <typename Image_type>
		struct Sharp_filter3x3<Image_type, 5> : public Kernel<Image_type, 3>
		{
			Sharp_filter3x3() : Kernel<Image_type, 3>({ -1.0f, -1.0f, -1.0f ,-1.0f, 9.0f, -1.0f ,-1.0f, -1.0f, -1.0f }) {}
		};


		template <typename Image_type, size_t type = 1>
		Image_type sharp_filter3x3(const Image_type& image)
		{
			Sharp_filter3x3<Image_type, type> sf;
			return sf.apply(image);
		}




		// ================ Sobel operator ====================
		template <typename Image_type>
		struct Sobel_y : public Kernel<Image_type, 3>
		{
			Sobel_y() : Kernel<Image_type, 3>({ 1, 0 , -1, 2, 0, -2, 1, 0 , -1 }) {}
		};


		template <typename Image_type>
		struct Sobel_x : public Kernel<Image_type, 3>
		{
			Sobel_x() : Kernel<Image_type, 3>({ 1, 2 , 1, 0, 0, 0, -1, -2 , -1 }) {}
		};

		template <typename Image_type>
		Image_type sobel(const Image_type& origin)
		{
			Image_type res(origin.width, origin.height);
			Sobel_y<Image_type> sobel_y;
			Sobel_x<Image_type> sobel_x;

			Image_type sob_x = sobel_y.apply(RGB2BW(origin));
			Image_type sob_y = sobel_x.apply(RGB2BW(origin));

			for (int i = 0; i < origin.width * origin.height; i++)
				res[i] = sob_x[i] + sob_y[i];

			return res;
		}

		template <typename Image_type>
		Image_type sobel_avg(const Image_type& origin)
		{
			Image_type res(origin.width, origin.height);
			Sobel_y<Image_type> sobel_y;
			Sobel_x<Image_type> sobel_x;

			Image_type sob_x = sobel_y.apply(RGB2BW(origin));
			Image_type sob_y = sobel_x.apply(RGB2BW(origin));

			for (int i = 0; i < origin.width * origin.height; i++)
				res[i] = (sob_x[i] / 2 + sob_y[i] / 2);

			return res;
		}


		// =============== Box filter =======================
		template <typename Image_type, size_t size = 3>
		struct Box_filter : public Kernel<Image_type, size>
		{
			using Kernel<Image_type, size>::Kernal_init;

			Box_filter()
			{
				float Kernel[size][size];
				float coef = 1.0f / (size * size);

				for (int i = 0; i < size; i++)
					for (int j = 0; j < size; j++)
						Kernel[i][j] = coef;

				Kernal_init(Kernel);
			}
		};


		template <typename Image_type, size_t size = 3>
		Image_type box_filter(const Image_type& image)
		{
			Box_filter<Image_type, size> bf;
			return bf.apply(image);
		}

	}
}
