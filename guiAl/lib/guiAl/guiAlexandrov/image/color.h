#pragma once

#include <cstdint>
#include <stdlib.h>


template <typename T>
T chanel_clip(T color);

template <>
float chanel_clip<float>(float color);

template <>
int chanel_clip<int>(int color);

template <>
uint8_t chanel_clip<uint8_t>(uint8_t color);



struct Color
{
	union
	{
		struct { uint8_t b, g, r, a; };
		struct { uint8_t Y, U, V; };
		uint8_t raw[4];
		uint32_t whole;
	};

	Color();
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	Color(uint8_t color);

	Color operator +(Color color);

	Color operator - (const Color& color) const;

	Color operator *(float f) const;

	Color operator /(float f) const;

	Color& operator += (const Color& other);

	Color& operator *=(float f);

	static uint8_t clip_chanel(int chanel);

};


struct fColor
{
	union
	{
		struct { float b, g, r, a; };
		struct { float Y, U, V; };
		float raw[4];
	};

	fColor();
	fColor(float r, float g, float b, float a = 1.0f);
	fColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	fColor(float color);
	fColor(Color color);


	fColor operator +(const fColor& f) const;

	fColor& operator +=(const fColor& f);

	fColor operator /(float f) const;

	fColor operator * (float f) const;

	fColor& operator *= (float f);

	static float chanel_clip(float chanel);

	operator Color() const;
};