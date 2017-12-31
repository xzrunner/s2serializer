#pragma once

#include <stdint.h>

namespace sns
{

// text		RGBA	old
// text		ARGB
// add		ABGR
// multi	BGRA
enum PIXEL_TYPE
{
	RGBA = 0,
	ARGB,
	ABGR,
	BGRA
};

class ColorParser
{
public:
	static uint32_t StringToRGBA(const char* str, size_t str_len, PIXEL_TYPE type);

	static uint32_t Trans(uint32_t src, PIXEL_TYPE src_type, PIXEL_TYPE dst_type);

}; // ColorParser

}

#include "sns/ColorParser.inl"