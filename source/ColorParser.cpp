#include "sns/ColorParser.h"

#include <guard/check.h>

#include <cstring>
#include <string>

namespace sns
{

static inline 
int char2hex(char c)
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	} else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	} else if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	} else {
		GD_REPORT_ASSERT("char2hex");
		return 0;
	}
}

static inline 
int char2channel(char high, char low)
{
	int col = char2hex(high) * 16 + char2hex(low);
	GD_ASSERT(col >= 0 && col <= 255, "char2channel");
	return col;
}

uint32_t ColorParser::StringToRGBA(const char* str, size_t str_len, PIXEL_TYPE type)
{
	if (str_len == 0) {
		return 0;
	}
	if (strncmp(str, "0xffffffff", str_len) == 0) {
		return 0xffffffff;
	}

	std::string snum(str, str_len);
	if (snum[0] != '0' || (snum[1] != 'x' && snum[1] != 'X')) {
		snum = "0x" + snum;
	}

	int len = snum.length();

	uint8_t r, g, b, a;
	if (len == 4)
	{
		if (type == RGBA || BGRA)
			a = char2channel(snum[2], snum[3]);
		else if (type == ARGB)
			b = char2channel(snum[2], snum[3]);
		else if (type == ABGR)
			r = char2channel(snum[2], snum[3]);
	}
	else if (len == 10)
	{
		if (type == RGBA) {
			r = char2channel(snum[2], snum[3]);
			g = char2channel(snum[4], snum[5]);
			b = char2channel(snum[6], snum[7]);
			a = char2channel(snum[8], snum[9]);
		} else if (type == ARGB) {
			a = char2channel(snum[2], snum[3]);
			r = char2channel(snum[4], snum[5]);
			g = char2channel(snum[6], snum[7]);
			b = char2channel(snum[8], snum[9]);
		} else if (type == ABGR) {
			a = char2channel(snum[2], snum[3]);
			b = char2channel(snum[4], snum[5]);
			g = char2channel(snum[6], snum[7]);
			r = char2channel(snum[8], snum[9]);
		}  else if (type == BGRA) {
			b = char2channel(snum[2], snum[3]);
			g = char2channel(snum[4], snum[5]);
			r = char2channel(snum[6], snum[7]);
			a = char2channel(snum[8], snum[9]);
		}
	}

	return r << 24 | g << 16 | b << 8 | a;
}

static inline
char hex2char(int v)
{
	assert(v >= 0 && v <= 15);
	if (v >= 0 && v <= 9)
		return '0' + v;
	else if (v >= 10 && v <= 15)
		return 'a' + (v - 10);
	else {
		assert(0);
		return '0';
	}
}

static inline
std::string channel2char(int col)
{
	assert(col >= 0 && col <= 255);

	int high = col / 16,
		low = col % 16;

	std::string ret;
	ret += hex2char(high);
	ret += hex2char(low);

	return ret;
}

std::string ColorParser::RGBAToString(uint32_t rgba, PIXEL_TYPE type)
{
	std::string ret = "0x";

	uint8_t r, g, b, a;
	r = (rgba >> 24) & 0xff;
	g = (rgba >> 16) & 0xff;
	b = (rgba >> 8) & 0xff;
	a = rgba & 0xff;

	if (type == sns::RGBA) {
		ret += channel2char(r);
		ret += channel2char(g);
		ret += channel2char(b);
		ret += channel2char(a);
	} else if (type == sns::ARGB) {
		ret += channel2char(a);
		ret += channel2char(r);
		ret += channel2char(g);
		ret += channel2char(b);
	} else if (type == sns::ABGR) {
		ret += channel2char(a);
		ret += channel2char(b);
		ret += channel2char(g);
		ret += channel2char(r);
	} else if (type == sns::BGRA) {
		ret += channel2char(b);
		ret += channel2char(g);
		ret += channel2char(r);
		ret += channel2char(a);
	}

	return ret;
}

}