

fImage auto_contrast(fImage& image, Histogram& hist)
{
	fImage res;
	res.resize(image.width, image.height);

	// cut 5% from bounds
	int cut_amount = 0.01f * image.height * image.width;
	int x_min, x_max;

	// find new bounds
	int count = 0;
	for (int i = 0; i < 256; i++)
	{
		count += hist.cbrightness[i];
		if (count >= cut_amount)
		{
			x_min = i;
			break;
		}
	}
	
	count = 0;
	for (int i = 255; i >= 0; i--)
	{
		count += hist.cbrightness[i];
		if (count >= cut_amount)
		{
			x_max = i;
			break;
		}
	}

	float c = 255.0f / (x_max - x_min);
	for (int i = 0; i < image.height * image.width; i++)
	{
		fColor& pixel = image[i];

		float Y = 0.2126f * pixel.r + 0.7152f * pixel.g + 0.0722 * pixel.b;
		float U = -0.0999f * pixel.r - 0.336f * pixel.g + 0.436f * pixel.b;
		float V = 0.615f * pixel.r - 0.5586f * pixel.g - 0.0563 * pixel.b;
		
		// corection
		Y = (Y - float(x_min) / 255.0f) * c;

		res[i].r = color_clipf(Y + 1.2803f * V);
		res[i].g = color_clipf(Y - 0.2148f * U - 0.4805f * V);
		res[i].b = color_clipf(Y + 2.1279f * U);
	}

	return res;
}