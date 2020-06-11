#pragma once

#include "../libs/thread_pool.h"
#include "color.h"

#ifndef MAX
#define MAX(a, b) (a > b? a: b)
#endif


namespace gui
{
	namespace cpu
	{

		template <typename T>
		void drawPixel(Image_base<T>& surface, int x, int y, const Color_base<T>& color)
		{
			if ((uint32_t)y >= surface.height || (uint32_t)x >= surface.width) return;
			surface[y * surface.width + x] = color;
		}

		template <typename T>
		void drawPixel(Image_base<T>& surface, int x, int y, const Color_base<T>& color, int width)
		{
			int left_width = width / 2;
			int half_width = (width + 1) / 2;

			for (int i = -left_width; i < half_width; i++)
				for (int j = -left_width; j < half_width; j++)
					drawPixel(surface, x + j, y + i, color);
		}


		template <typename T>
		void draw_filled_circle(Image_base<T>& surface, int x, int y, const Color_base<T>& color, int radius)
		{
			for (int i = -radius; i < radius; i++)
			{
				for (int j = -radius; j < radius; j++)
				{
					if (i * i + j * j < radius * radius)
						drawPixel(surface, x + i, y + j, color);
				}
			}
		}



		template <typename T>
		void drawLine(Image_base<T>& surface, int x, int y, int x2, int y2, const Color_base<T>& color, int width = 1)
		{
			bool yLonger = false;
			int shortLen = y2 - y;
			int longLen = x2 - x;
			if (abs(shortLen) > abs(longLen)) {
				int swap = shortLen;
				shortLen = longLen;
				longLen = swap;
				yLonger = true;
			}
			int decInc;
			if (longLen == 0) decInc = 0;
			else decInc = (shortLen << 16) / longLen;

			if (yLonger) {
				if (longLen > 0) {
					longLen += y;
					for (int j = 0x8000 + (x << 16); y <= longLen; ++y) {
						drawPixel(surface, j >> 16, y, color, width);
						j += decInc;
					}
					return;
				}
				longLen += y;
				for (int j = 0x8000 + (x << 16); y >= longLen; --y) {
					drawPixel(surface, j >> 16, y, color, width);
					j -= decInc;
				}
				return;
			}

			if (longLen > 0) {
				longLen += x;
				for (int j = 0x8000 + (y << 16); x <= longLen; ++x) {
					drawPixel(surface, x, j >> 16, color, width);
					j += decInc;
				}
				return;
			}
			longLen += x;
			for (int j = 0x8000 + (y << 16); x >= longLen; --x) {
				drawPixel(surface, x, j >> 16, color, width);
				j -= decInc;
			}
		}


		template <typename T>
		void draw_line(Image_base<T>& surface, float fx0, float fy0, float fx1, float fy1, const Color_base<T>& color, int width = 1)
		{
			int x0 = fx0 * surface.width;
			int y0 = fy0 * surface.height;
			int x1 = fx1 * surface.width;
			int y1 = fy1 * surface.height;
			drawLine(surface, x0, y0, x1, y1, color, width);
		}


		template <typename T>
		void draw_filled_circle(Image_base<T>& surface, float fx, float fy, const Color_base<T>& color, float fradius)
		{
			int x = fx * surface.width;
			int y = fy * surface.height;
			int radius = (surface.width / 2) * fradius;
			if ((uint32_t)y >= surface.height || (uint32_t)x >= surface.width) return;

			for (int i = -radius; i < radius; i++)
				for (int j = -radius; j < radius; j++)
					if (i * i + j * j < radius * radius)
						drawPixel(surface, x + i, y + j, color);
		}


		template <typename T>
		void draw_rect(Image_base<T>& surface, float fx, float fy, float fwidth, float fheight, const Color_base<T>& color, int pixel_width = 1)
		{
			int x0 = surface.width * fx;
			int y0 = surface.height * fy;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			drawLine(surface, x0, y0, x0 + width, y0, color, pixel_width);
			drawLine(surface, x0, y0, x0, y0 + height, color, pixel_width);
			drawLine(surface, x0 + width, y0, x0 + width, y0 + height, color, pixel_width);
			drawLine(surface, x0, y0 + height, x0 + width, y0 + height, color, pixel_width);
		}

		template <typename T>
		void draw_filled_rect(Image_base<T>& surface, float fx, float fy, float fwidth, float fheight, const Color_base<T>& color)
		{
			int x0 = surface.width * fx;
			int y0 = surface.height * fy;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			for (int y = y0; y < y0 + height; y++)
				for (int x = x0; x < x0 + width; x++)
					drawPixel(surface, x, y, color);
		}

		template <typename T>
		void draw_filled_rect_async(Image_base<T>& surface, float fx, float fy, float fwidth, float fheight, const Color_base<T>& color)
		{
			if (fx > 1.1f || fy > 1.1f || fx + fwidth < 0.0f || fy + fheight < 0.0f) return;

			int x0 = surface.width * fx;
			int y0 = surface.height * fy;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			
			parallel_for(0, height, true,
				[height, width, &surface, &color](int from_y, int to_y)
				{
						for (int y = from_y; y < to_y; y++)
							for (int x = 0; x < width; x++)
								drawPixel(surface, x, y, color);
				}
			);

		}



		// ============= Draw Image to the surface ===================

		template <typename T, typename S>
		void draw_image(Image_base<T>& surface, const Image_base<S>& image,
			float fpos_x, float fpos_y, float fwidth, float fheight)
		{
			if (fpos_x > 1.1f || fpos_y > 1.1f || fpos_x + fwidth < 0.0f || fpos_y + fheight < 0.0f || !image.valid()) return;

			int pos_x = surface.width * fpos_x;
			int pos_y = surface.height * fpos_y;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					const Color_base<T>& color = image.get_pixel_scaled(x, y, width, height);
					drawPixel(surface, x + pos_x, y + pos_y, color);
				}
			}
		}


		template <typename T, typename S>
		void draw_image_async(Image_base<T>& surface, const Image_base<S>& image,
			float fpos_x, float fpos_y, float fwidth, float fheight)
		{
			if (fpos_x > 1.1f || fpos_y > 1.1f || fpos_x + fwidth < 0.0f || fpos_y + fheight < 0.0f || !image.valid()) return;

			int pos_x = surface.width * fpos_x;
			int pos_y = surface.height * fpos_y;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			
			parallel_for(0, height, true, 
				[pos_y, pos_x, height, width, &surface, &image](int from_y, int to_y)
				{
					for (int y = from_y; y < to_y; y++)
					{
						for (int x = 0; x < width; x++)
						{
							const Color_base<T>& color = image.get_pixel_scaled(x, y, width, height);
							drawPixel(surface, x + pos_x, y + pos_y, color);
						}
					}
				}
			);

		}

		// no bound cheking here, if image outside the canvas it wouldn'd 
		template <typename T, typename S>
		void draw_image_async_direct(Image_base<T>& surface, const Image_base<S>& image,
			float fpos_x, float fpos_y, float fwidth, float fheight)
		{
			if (fpos_x > 1.0f || fpos_y > 1.0f || fpos_x < 0.0f || fpos_y < 0.0f || image.invalid) return;

			int pos_x = surface.width * fpos_x;
			int pos_y = surface.height * fpos_y;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			parallel_for(0, height, true,
				[pos_y, pos_x, height, width, &surface, &image](int from_y, int to_y)
				{
					for (int y = from_y; y < to_y; y++)
					{
						for (int x = 0; x < width; x++)
						{
							assert(x < surface.width);
							const Color_base<T>& color = image.get_pixel_scaled(x, y, width, height);
							surface[(y + pos_y) * surface.width + (x + pos_x)] = color;
						}
					}
				}
			);

		}




		// ======================= alpha blending ==============================

		template <typename T>
		void drawPixel_a(Image_base<T>& surface, int x, int y, const fColor& color)
		{
			if ((uint32_t)y >= surface.height || (uint32_t)x >= surface.width) return;
			fColor dest = surface[y * surface.width + x];
			dest = color * color.a + dest * (dest.a * (1.0f - color.a));
			surface[y * surface.width + x] = dest;
		}

		template <typename T>
		void drawPixel_a(Image_base<T>& surface, int x, int y, const fColor& color, int width)
		{
			int half_width = (width + 1) / 2;

			for (int i = -half_width; i < half_width; i++)
				for (int j = -half_width; j < half_width; j++)
					drawPixel_a(surface, x + j, y + i, color);
		}


		template <typename T>
		void draw_filled_circle_a(Image_base<T>& surface, float fx, float fy, const fColor& color, float fradius)
		{
			int x = fx * surface.width;
			int y = fy * surface.height;
			int radius = (surface.width / 2) * fradius;
			for (int i = -radius; i < radius; i++)
			{
				for (int j = -radius; j < radius; j++)
				{
					if (i * i + j * j < radius * radius)
						drawPixel_a(surface, x + i, y + j, color);
				}
			}
		}

		template <typename T>
		void draw_filled_circle_a(Image_base<T>& surface, int x, int y, const fColor& color, int radius)
		{
			for (int i = -radius; i < radius; i++)
			{
				for (int j = -radius; j < radius; j++)
				{
					if (i * i + j * j < radius * radius)
						drawPixel_a(surface, x + i, y + j, color);
				}
			}
		}


		template <typename T>
		void drawLine_a(Image_base<T>& surface, int x, int y, int x2, int y2, fColor color, int width = 1)
		{
			color.a /= (width / 3 + 1);
			bool yLonger = false;
			int shortLen = y2 - y;
			int longLen = x2 - x;
			if (abs(shortLen) > abs(longLen)) {
				int swap = shortLen;
				shortLen = longLen;
				longLen = swap;
				yLonger = true;
			}
			int decInc;
			if (longLen == 0) decInc = 0;
			else decInc = (shortLen << 16) / longLen;

			if (yLonger) {
				if (longLen > 0) {
					longLen += y;
					for (int j = 0x8000 + (x << 16); y <= longLen; ++y) {
						drawPixel_a(surface, j >> 16, y, color, width);
						j += decInc;
					}
					return;
				}
				longLen += y;
				for (int j = 0x8000 + (x << 16); y >= longLen; --y) {
					drawPixel_a(surface, j >> 16, y, color, width);
					j -= decInc;
				}
				return;
			}

			if (longLen > 0) {
				longLen += x;
				for (int j = 0x8000 + (y << 16); x <= longLen; ++x) {
					drawPixel_a(surface, x, j >> 16, color, width);
					j += decInc;
				}
				return;
			}
			longLen += x;
			for (int j = 0x8000 + (y << 16); x >= longLen; --x) {
				drawPixel_a(surface, x, j >> 16, color, width);
				j -= decInc;
			}
		}

		template <typename T>
		void draw_line_a(Image_base<T>& surface, float fx0, float fy0, float fx1, float fy1, fColor color, int width = 1)
		{
			int x0 = fx0 * surface.width;
			int y0 = fy0 * surface.height;
			int x1 = fx1 * surface.width;
			int y1 = fy1 * surface.height;
			drawLine_a(surface, x0, y0, x1, y1, color, width);
		}




		template <typename T>
		void draw_rect_a(Image_base<T>& surface, float fx, float fy, float fwidth, float fheight, fColor color, int pixel_width = 1)
		{
			if (fx > 1.1f || fy > 1.1f || fx + fwidth < 0.0f || fy + fheight < 0.0f) return;

			int x0 = surface.width * fx;
			int y0 = surface.height * fy;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			drawLine_a(surface, x0, y0, x0 + width, y0, color, pixel_width);
			drawLine_a(surface, x0, y0, x0, y0 + height, color, pixel_width);
			drawLine_a(surface, x0 + width, y0, x0 + width, y0 + height, color, pixel_width);
			drawLine_a(surface, x0, y0 + height, x0 + width, y0 + height, color, pixel_width);
		}

		template <typename T>
		void draw_filled_rect_a(Image_base<T>& surface, float fx, float fy, float fwidth, float fheight, fColor color)
		{
			if (fx > 1.1f || fy > 1.1f || fx + fwidth < 0.0f || fy + fheight < 0.0f) return;

			int x0 = surface.width * fx;
			int y0 = surface.height * fy;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			for (int y = y0; y < y0 + height; y++)
				for (int x = x0; x < width; x++)
					drawPixel_a(surface, x, y, color);
		}

		template <typename T>
		void draw_filled_rect_async_a(Image_base<T>& surface, float fx, float fy, float fwidth, float fheight, fColor color)
		{
			if (fx > 1.1f || fy > 1.1f || fx + fwidth < 0.0f || fy + fheight < 0.0f) return;

			int x0 = surface.width * fx;
			int y0 = surface.height * fy;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			parallel_for(0, height, true,
				[x0, height, width, &surface, &color](int from_y, int to_y)
				{
					for (int y = from_y; y < to_y; y++)
						for (int x = x0; x < width + x0; x++)
							drawPixel_a(surface, x, y, color);
				}
			);

		}



		// =============== image alpha blending ====================


		template <typename T, typename S>
		void draw_image_a(Image_base<T>& surface, const Image_base<S>& image,
			float fpos_x, float fpos_y, float fwidth, float fheight)
		{
			if (fpos_x > 1.1f || fpos_y > 1.1f || fpos_x + fwidth < 0.0f || fpos_y + fheight < 0.0f || image.invalid()) return;

			int pos_x = surface.width * fpos_x;
			int pos_y = surface.height * fpos_y;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					const fColor& color = image.get_pixel_scaled(x, y, width, height);
					drawPixel_a(surface, x + pos_x, y + pos_y, color);
				}
			}
		}


		template <typename T, typename S>
		void draw_image_async_a(Image_base<T>& surface, const Image_base<S>& image,
			float fpos_x, float fpos_y, float fwidth, float fheight)
		{
			if (fpos_x > 1.1f || fpos_y > 1.1f || fpos_x + fwidth < 0.0f || fpos_y + fheight < 0.0f || !image.valid()) return;

			int pos_x = surface.width * fpos_x;
			int pos_y = surface.height * fpos_y;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			
			parallel_for(0, height, true,
				[pos_y, pos_x, height, width, &surface, &image](int from_y, int to_y)
				{
					for (int y = from_y; y < to_y; y++)
					{
						for (int x = 0; x < width; x++)
						{
							const fColor& color = image.get_pixel_scaled(x, y, width, height);
							drawPixel_a(surface, x + pos_x, y + pos_y, color);
						}
					}
				}
			);

		}


		template <typename T, typename S>
		void draw_image_a(Image_base<T>& surface, const Image_base<S>& image,
			float fpos_x, float fpos_y, float fwidth, float fheight, float alpha)
		{
			if (fpos_x > 1.1f || fpos_y > 1.1f || fpos_x + fwidth < 0.0f || fpos_y + fheight < 0.0f || image.invalid) return;

			int pos_x = surface.width * fpos_x;
			int pos_y = surface.height * fpos_y;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					const fColor& color = image.get_pixel_scaled(x, y, width, height);
					color.a = alpha;
					drawPixel_a(surface, x + pos_x, y + pos_y, color);
				}
			}
		}


		template <typename T, typename S>
		void draw_image_async_a(Image_base<T>& surface, const Image_base<S>& image,
			float fpos_x, float fpos_y, float fwidth, float fheight, float alpha)
		{
			if (fpos_x > 1.1f || fpos_y > 1.1f || fpos_x + fwidth < 0.0f || fpos_y + fheight < 0.0f || !image.valid()) return;

			int pos_x = surface.width * fpos_x;
			int pos_y = surface.height * fpos_y;


			int width = surface.width * fwidth;
			int height = surface.height * fheight;


				parallel_for(0, height, true,
					[alpha, pos_y, pos_x, height, width, &surface, &image](int from, int to)
					{
						for (int y = from; y < to; y++)
						{
							for (int x = 0; x < width; x++)
							{
								const fColor& color = image.get_pixel_scaled(x, y, width, height);
								color.a = alpha;
								drawPixel_a(surface, x + pos_x, y + pos_y, color);
							}
						}
					}
				);

		}


		// ============================= rotate ========================================

		template <typename T, typename S>
		void draw_image_a_rotate(Image_base<T>& surface, const Image_base<S>& image,
			float fpos_x, float fpos_y, float fwidth, float fheight, float angle)
		{
			if (fpos_x > 1.1f || fpos_y > 1.1f || fpos_x + fwidth < 0.0f || fpos_y + fheight < 0.0f || image.invalid) return;

			int pos_x = surface.width * fpos_x;
			int pos_y = surface.height * fpos_y;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			int center_x = width / 2;
			int center_y = height / 2;

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int rotate_x = cosf(angle) * (x - center_x) + sinf(angle) * (y - center_y);
					int rotate_y = -sinf(angle) * (x - center_x) + cosf(angle) * (y - center_y);

					const fColor& color = image.get_pixel_scaled(x, y, width, height);
					drawPixel_a(surface, rotate_x + pos_x, rotate_y + pos_y, color);
				}
			}
		}

		template <typename T, typename S>
		void draw_image_async_a_rotate(Image_base<T>& surface, const Image_base<S>& image,
			float fpos_x, float fpos_y, float fwidth, float fheight, float angle)
		{
			if (fpos_x > 1.1f || fpos_y > 1.1f || fpos_x + fwidth < 0.0f || fpos_y + fheight < 0.0f || image.invalid) return;

			int pos_x = surface.width * fpos_x;
			int pos_y = surface.height * fpos_y;

			int width = surface.width * fwidth;
			int height = surface.height * fheight;

			int center_x = width / 2;
			int center_y = height / 2;


			parallel_for(0, height, true,
				[pos_y, pos_x, height, width, center_x, center_y, angle, &surface, &image](int from, int to)
				{
					for (int y = from; y < to; y++)
					{
						for (int x = 0; x < width; x++)
						{
							int rotate_x = cosf(angle) * (x - center_x) + sinf(angle) * (y - center_y);
							int rotate_y = -sinf(angle) * (x - center_x) + cosf(angle) * (y - center_y);

							const fColor& color = image.get_pixel_scaled(x, y, width, height);
							drawPixel_a(surface, rotate_x + pos_x, rotate_y + pos_y, color);
						}
					}
				}
			);

		}

	}
}