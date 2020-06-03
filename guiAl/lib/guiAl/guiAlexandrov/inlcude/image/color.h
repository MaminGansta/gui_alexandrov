#pragma once

#include <cstdint>
#include <stdlib.h>


namespace gui
{

	template <typename T>
	T chanel_clip(T color);

	template <>
	float chanel_clip<float>(float color);

	template <>
	int chanel_clip<int>(int color);

	template <>
	uint8_t chanel_clip<uint8_t>(uint8_t color);


	template <typename chanel_type>
	struct Color_base
	{
		union
		{
			struct { chanel_type b, g, r, a; };
			struct { chanel_type Y, U, V; };
			chanel_type raw[4];
		};
	};


	typedef Color_base<uint8_t> Color;

	// Color
	template <>
	struct Color_base<uint8_t>
	{
		union
		{
			struct { uint8_t b, g, r, a; };
			struct { uint8_t Y, U, V; };
			uint8_t raw[4];
			uint32_t whole;
		};

		Color_base();
		Color_base(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		Color_base(uint8_t color);

		
		Color& operator += (const Color& other);

		Color& operator *=(float f);


		Color operator +(const Color& color) const;

		Color operator - (const Color& color) const;

		Color operator *(float f) const;

		Color operator /(float f) const;

		
		static uint8_t clip_chanel(int chanel);

	};



	// fColor
	template <>
	struct Color_base<float>
	{
		union
		{
			struct { float b, g, r, a; };
			struct { float Y, U, V; };
			float raw[4];
		};

		Color_base();
		Color_base(float r, float g, float b, float a = 1.0f);
		Color_base(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		Color_base(float color);
		Color_base(const Color_base<uint8_t>& color);


		Color_base<float>& operator +=(const Color_base<float>& f);

		Color_base<float>& operator *= (float f);


		Color_base<float> operator +(const Color_base<float>& f) const;

		Color_base<float> operator /(float f) const;

		Color_base<float> operator * (float f) const;


		static float chanel_clip(float chanel);

		operator Color_base<uint8_t>() const;
	};

	typedef Color_base<float> fColor;

}
