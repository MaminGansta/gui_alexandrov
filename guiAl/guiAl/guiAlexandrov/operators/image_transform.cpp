
fImage RGB2YCbCr(const fImage& origin)
{
	fImage res(origin.width, origin.height);

	for (int i = 0; i < origin.width * origin.height; i++)
	{
		fColor pixel = origin.data[i];

		res[i].Y = 0.2126f * pixel.r + 0.7152f * pixel.g + 0.0722 * pixel.b;
		res[i].U = -0.0999f * pixel.r - 0.336f * pixel.g + 0.436f * pixel.b;
		res[i].V = 0.615f * pixel.r - 0.5586f * pixel.g - 0.0563 * pixel.b;
	}

	return res;
}


fImage YCbCr2RGB(const fImage& origin)
{
	fImage res(origin.width, origin.height);

	for (int i = 0; i < origin.width * origin.height; i++)
	{
		fColor pixel = origin.data[i];

		res[i].r = color_clipf(pixel.Y + 1.2803f * pixel.V);
		res[i].g = color_clipf(pixel.Y - 0.2148f * pixel.U - 0.4805f * pixel.V);
		res[i].b = color_clipf(pixel.Y + 2.1279f * pixel.U);
	}

	return res;
}