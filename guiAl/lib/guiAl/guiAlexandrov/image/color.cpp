
#include "image/color.h"


namespace gui
{

	template <>
	float chanel_clip<float>(float color) { return __min(__max(color, 0.0f), 1.0f); }

	template <>
	int chanel_clip<int>(int color) { return __min(__max(color, 0), 255); }

	template <>
	uint8_t chanel_clip<uint8_t>(uint8_t color) { return __min(__max(color, 0), 255); }



	Color::Color_base() : r(0), g(0), b(0), a(255) {}
	Color::Color_base(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
	Color::Color_base(uint8_t color) : r(color), g(color), b(color), a(255) {}


	Color Color::operator +(const Color& color) const
	{
		return Color(__min(r + color.r, 255), __min(g + color.g, 255), __min(b + color.b, 255));
	}

	Color Color::operator- (const Color& color) const
	{
		return Color(r - color.r, g - color.g, b - color.b);
	}

	Color Color::operator*(float f) const
	{
		return Color(r * f, g * f, b * f);
	}

	Color Color::operator /(float f) const
	{
		return Color(r / f, g / f, b / f);
	}

	Color& Color::operator += (const Color& other)
	{
		r = __min(r + other.r, 255);
		g = __min(g + other.g, 255);
		b = __min(b + other.b, 255);
		return *this;
	}

	Color& Color::operator *=(float f)
	{
		r *= f;
		g *= f;
		b *= f;
		return *this;
	}

	uint8_t Color::clip_chanel(int chanel)
	{
		return __max(__min(chanel, 255), 0);
	}



	// Color

	fColor::Color_base() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
	fColor::Color_base(float r, float g, float b, float) : r(r), g(g), b(b), a(a) {}
	fColor::Color_base(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(float(r) / 255.0f), g(float(g) / 255.0f), b(float(b) / 255.0f), a(float(a) / 255.0f) {}
	fColor::Color_base(float color) : r(color), g(color), b(color), a(1.0f) {}
	fColor::Color_base(const Color& color) : r(float(color.r) / 255.0f), g(float(color.g) / 255.0f), b(float(color.b) / 255.0f), a(float(color.a) / 255.0f) {}


	fColor fColor::operator +(const fColor& f) const
	{
		return fColor(r + f.r, g + f.g, b + f.b);
	}

	fColor& fColor::operator +=(const fColor& f)
	{
		r = __min(1.0f, r + f.r);
		g = __min(1.0f, g + f.g);
		b = __min(1.0f, b + f.b);
		return *this;
	}

	fColor fColor::operator /(float f) const
	{
		return fColor(r / f, g / f, b / f);
	}

	fColor fColor::operator*(float f) const
	{
		return fColor(__min(1.0f, r * f), __min(1.0f, g * f), __min(1.0f, b * f));
	}

	fColor& fColor::operator *= (float f)
	{
		r = __min(1.0f, r * f);
		g = __min(1.0f, g * f);
		b = __min(1.0f, b * f);
		return *this;
	}

	float fColor::chanel_clip(float chanel)
	{
		return __max(__min(chanel, 1.0f), 0.0f);
	}

	fColor::operator Color() const
	{
		return Color(__min(r * 255.0f, 255.0f), __min(g * 255.0f, 255.0f), __min(b * 255.0f, 255.0f), __min(a * 255.0f, 255.0f));
	}

}