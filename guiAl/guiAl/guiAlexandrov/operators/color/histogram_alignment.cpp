
fImage hist_alignment(fImage& image, Histogram hist)
{
	fImage res;
	res.resize(image.width, image.height);

	uint32_t cdf[256];
	uint32_t cdf_min;
	uint32_t count = 0;

	cdf[0] = hist.cbrightness[0];
	for (int i = 1; i < 256; i++)
	{
		cdf[i] = cdf[i - 1] + hist.cbrightness[i];
		count += hist.cbrightness[i];
	}

	for (int i = 0; i < 255; i++)
	{
		if (cdf[i] > 0)
		{
			cdf_min = cdf[i];
			break;
		}
	}

	for (int i = 0; i < image.height * image.width; i++)
	{
		fColor& pixel = image[i];

		float Y = 0.2126f * pixel.r + 0.7152f * pixel.g + 0.0722 * pixel.b;
		float U = -0.0999f * pixel.r - 0.336f * pixel.g + 0.436f * pixel.b;
		float V = 0.615f * pixel.r - 0.5586f * pixel.g - 0.0563 * pixel.b;

		// corection
		Y = float(cdf[int(Y * 255.0f)] - cdf_min) / (count - 1);
		//float a = float(cdf[int(Y * 255.0f)] - cdf_min) / (count - 1);


		res[i].r = color_clipf(Y + 1.2803f * V);
		res[i].g = color_clipf(Y - 0.2148f * U - 0.4805f * V);
		res[i].b = color_clipf(Y + 2.1279f * U);
	}

	return res;
}