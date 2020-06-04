#pragma once

#include <stdint.h>
#include "../image/image.h"
#include "histogram.h"



namespace gui
{
	namespace cpu
	{

		template <typename T>
		Image_base<T> gray_world(const Image_base<T>& image, const Histogram<T>& hist)
		{
			Image_base<T> res;
			res.resize(image.width, image.height);

			uint32_t R = 0;
			uint32_t G = 0;
			uint32_t B = 0;
			uint32_t AVG = 0;
			uint32_t count = 0;

			for (int i = 0; i < 256; i++)
			{
				count += hist.cred[i];
				R += i * hist.cred[i];
				G += i * hist.cgreen[i];
				B += i * hist.cblue[i];
			}

			AVG = (R + G + B) / 3;

			float rw = float(AVG) / R;
			float gw = float(AVG) / G;
			float bw = float(AVG) / B;

			for (int i = 0; i < image.height * image.width; i++)
			{
				res[i].r = clip(image[i].r * rw);
				res[i].g = clip(image[i].g * gw);
				res[i].b = clip(image[i].b * bw);
			}

			return res;
		}


		template <typename T>
		Image_base<T> gray_world(const Image_base<T>& image)
		{
			Histogram<T> hist(image);

			Image_base<T> res;
			res.resize(image.width, image.height);

			uint32_t R = 0;
			uint32_t G = 0;
			uint32_t B = 0;
			uint32_t AVG = 0;
			uint32_t count = 0;

			for (int i = 0; i < 256; i++)
			{
				count += hist.cred[i];
				R += i * hist.cred[i];
				G += i * hist.cgreen[i];
				B += i * hist.cblue[i];
			}

			AVG = (R + G + B) / 3;

			float rw = float(AVG) / R;
			float gw = float(AVG) / G;
			float bw = float(AVG) / B;

			for (int i = 0; i < image.height * image.width; i++)
			{
				res[i].r = clip(image[i].r * rw);
				res[i].g = clip(image[i].g * gw);
				res[i].b = clip(image[i].b * bw);
			}

			return res;
		}
	}

}