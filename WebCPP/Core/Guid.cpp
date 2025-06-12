
#include "WebCPP/Core/Guid.h"
#include <stdexcept>

Guid Guid::fromString(const std::string& value)
{
	if (value.size() != 36 || value[8] != '-' || value[13] != '-' || value[18] != '-' || value[23] != '-')
		throw std::runtime_error("Invalid guid");

	Guid guid;
	uint8_t* d = (uint8_t*)guid.data;
	for (int i = 0; i < 8; i += 2)
		d[i >> 1] = hexValue(value.data() + i);
	for (int i = 9; i < 13; i += 2)
		d[(i - 1) >> 1] = hexValue(value.data() + i);
	for (int i = 14; i < 18; i += 2)
		d[(i - 2) >> 1] = hexValue(value.data() + i);
	for (int i = 19; i < 23; i += 2)
		d[(i - 3) >> 1] = hexValue(value.data() + i);
	for (int i = 24; i < 36; i += 2)
		d[(i - 4) >> 1] = hexValue(value.data() + i);
	return guid;
}

std::string Guid::toString() const
{
	// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
	char str[36];
	str[8] = '-';
	str[13] = '-';
	str[18] = '-';
	str[23] = '-';
	const uint8_t* d = (uint8_t*)data;
	for (int i = 0; i < 4; i++)
		hexString(d[i], str + (i << 1));
	for (int i = 4; i < 6; i++)
		hexString(d[i], str + (i << 1) + 1);
	for (int i = 6; i < 8; i++)
		hexString(d[i], str + (i << 1) + 2);
	for (int i = 8; i < 10; i++)
		hexString(d[i], str + (i << 1) + 3);
	for (int i = 10; i < 16; i++)
		hexString(d[i], str + (i << 1) + 4);
	return std::string(str, 36);
}

uint8_t Guid::hexValue(const char* s)
{
	uint8_t v = 0;
	for (int i = 0; i < 2; i++)
	{
		uint8_t val = 0;
		if (s[i] >= '0' && s[i] <= '9')
		{
			val = s[i] - '0';
		}
		else if (s[i] >= 'a' && s[i] <= 'f')
		{
			val = s[i] - 'a' + 10;
		}
		else if (s[i] >= 'A' && s[i] <= 'F')
		{
			val = s[i] - 'A' + 10;
		}
		v <<= 4;
		v |= val;
	}
	return v;
}

void Guid::hexString(uint8_t v, char* out)
{
	uint8_t val0 = (v >> 4) & 15;
	uint8_t val1 = v & 15;
	out[0] = val0 < 10 ? '0' + val0 : 'a' + val0 - 10;
	out[1] = val1 < 10 ? '0' + val1 : 'a' + val1 - 10;
}
