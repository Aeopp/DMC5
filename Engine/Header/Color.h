#pragma once
#ifndef _COLOR_H_
#define _COLOR_H_
#include "EngineDefine.h"
#include "EngineTypedef.h"

class ENGINE_DLL Color
{
public:
	float r, g, b, a;

	Color();
	Color(float _r, float _g, float _b, float _a);
	Color(uint32_t argb32);

	Color& operator =(const Color& other);
	Color& operator *=(const Color& other);

	static Color Lerp(const Color& from, const Color& to, float frac);
	static Color sRGBToLinear(uint32_t argb32);
	static Color sRGBToLinear(uint8_t red, uint8_t green, uint8_t blue);
	static Color sRGBToLinear(float red, float green, float blue, float alpha);

	static inline uint8_t ArgbA32(uint32_t c) { return ((uint8_t)((c >> 24) & 0xff)); }
	static inline uint8_t ArgbR32(uint32_t c) { return ((uint8_t)((c >> 16) & 0xff)); }
	static inline uint8_t ArgbG32(uint32_t c) { return ((uint8_t)((c >> 8) & 0xff)); }
	static inline uint8_t ArgbB32(uint32_t c) { return ((uint8_t)(c & 0xff)); }

	operator uint32_t() const;

	inline operator float* () { return &r; }
	inline operator const float* () const { return &r; }
};

#endif // !_COLOR_H_




