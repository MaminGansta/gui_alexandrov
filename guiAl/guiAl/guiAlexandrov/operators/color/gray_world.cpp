

fImage gray_world(fImage& image, Histogram hist)
{
	fImage res;
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
		res[i].r = min(image[i].r * rw, 1.0f);
		res[i].g = min(image[i].g * gw, 1.0f);
		res[i].b = min(image[i].b * bw, 1.0f);
	}

	return res;
}