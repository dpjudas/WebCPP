#pragma once

#include <string>
#include <cstring>

class Guid
{
public:
	static Guid fromString(const std::string& value);

	bool isNull() const { return data[0] == 0 && data[1] == 0; }

	std::string toString() const;

	bool operator<(const Guid& other) const { return std::memcmp(data, other.data, 16) < 0; }
	bool operator<=(const Guid& other) const { return std::memcmp(data, other.data, 16) <= 0; }
	bool operator>(const Guid& other) const { return std::memcmp(data, other.data, 16) > 0; }
	bool operator>=(const Guid& other) const { return std::memcmp(data, other.data, 16) >= 0; }
	bool operator==(const Guid& other) const { return std::memcmp(data, other.data, 16) == 0; }
	bool operator!=(const Guid& other) const { return std::memcmp(data, other.data, 16) != 0; }

	uint64_t data[2] = { 0, 0 };

private:
	static uint8_t hexValue(const char* s);
	static void hexString(uint8_t v, char* out);
};

template <>
struct std::hash<Guid>
{
	std::size_t operator()(const Guid& k) const
	{
		return k.data[0] ^ k.data[1];
	}
};
