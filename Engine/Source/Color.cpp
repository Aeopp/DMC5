#include "Color.h"

USING(ENGINE)


Color::Color()
	: r(0), g(0), b(0), a(1)
{
}

Color::Color(float _r, float _g, float _b, float _a)
	: r(_r), g(_g), b(_b), a(_a)
{
}

Color::Color(uint32_t argb32)
{
	a = ArgbA32(argb32) / 255.0f;
	r = ArgbR32(argb32) / 255.0f;
	g = ArgbG32(argb32) / 255.0f;
	b = ArgbB32(argb32) / 255.0f;
}

Color& Color::operator =(const Color& other)
{
	r = other.r;
	g = other.g;
	b = other.b;
	a = other.a;

	return *this;
}

Color& Color::operator *=(const Color& other)
{
	r *= other.r;
	g *= other.g;
	b *= other.b;
	a *= other.a;

	return *this;
}

Color Color::Lerp(const Color& from, const Color& to, float frac)
{
	Color ret;

	ret.r = from.r * (1 - frac) + to.r * frac;
	ret.g = from.g * (1 - frac) + to.g * frac;
	ret.b = from.b * (1 - frac) + to.b * frac;
	ret.a = from.a * (1 - frac) + to.a * frac;

	return ret;
}

Color Color::sRGBToLinear(uint32_t argb32)
{
	Color ret = sRGBToLinear(ArgbR32(argb32), ArgbG32(argb32), ArgbB32(argb32));
	ret.a = ArgbA32(argb32) / 255.0f;

	return ret;
}

Color Color::sRGBToLinear(uint8_t red, uint8_t green, uint8_t blue)
{
	Color ret;

	float lo_r = (float)red / 3294.6f;
	float lo_g = (float)green / 3294.6f;
	float lo_b = (float)blue / 3294.6f;

	float hi_r = powf((red / 255.0f + 0.055f) / 1.055f, 2.4f);
	float hi_g = powf((green / 255.0f + 0.055f) / 1.055f, 2.4f);
	float hi_b = powf((blue / 255.0f + 0.055f) / 1.055f, 2.4f);

	ret.r = (red < 10 ? lo_r : hi_r);
	ret.g = (green < 10 ? lo_g : hi_g);
	ret.b = (blue < 10 ? lo_b : hi_b);
	ret.a = 1;

	return ret;
}

Color Color::sRGBToLinear(float red, float green, float blue, float alpha)
{
	Color ret;

	float lo_r = (float)red / 12.92f;
	float lo_g = (float)green / 12.92f;
	float lo_b = (float)blue / 12.92f;

	float hi_r = powf((red + 0.055f) / 1.055f, 2.4f);
	float hi_g = powf((green + 0.055f) / 1.055f, 2.4f);
	float hi_b = powf((blue + 0.055f) / 1.055f, 2.4f);

	ret.r = (red < 0.04f ? lo_r : hi_r);
	ret.g = (green < 0.04f ? lo_g : hi_g);
	ret.b = (blue < 0.04f ? lo_b : hi_b);
	ret.a = alpha;

	return ret;
}

Color::operator uint32_t() const
{
	uint8_t _a = (uint8_t)(a * 255);
	uint8_t _r = (uint8_t)(r * 255);
	uint8_t _g = (uint8_t)(g * 255);
	uint8_t _b = (uint8_t)(b * 255);

	return (_a << 24) | (_r << 16) | (_g << 8) | _b;
}

