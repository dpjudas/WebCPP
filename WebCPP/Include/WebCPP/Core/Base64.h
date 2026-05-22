#pragma once

#include <string>
#include <vector>

namespace web
{
	class Base64
	{
	public:
		static std::string encode(const void* data, size_t size);
		static std::string encode(const std::string& data);
		static std::string encode(const std::vector<uint8_t>& data);

		static std::vector<uint8_t> decode(const std::string& data);
		static std::string decodeText(const std::string& data);
	};
}
