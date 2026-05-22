
#include "WebCPP/Core/Base64.h"

namespace web
{
	static const uint8_t char_to_base64[256] = {
		64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
		64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
		64,64,64,64,64,64,64,64,64,64,64,62,64,64,64,63,
		52,53,54,55,56,57,58,59,60,61,64,64,64,64,64,64,
		64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
		15,16,17,18,19,20,21,22,23,24,25,64,64,64,64,64,
		64,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
		41,42,43,44,45,46,47,48,49,50,51,64,64,64,64,64,
		64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
		64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
		64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
		64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
		64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
		64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
		64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
		64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
	};

	static const char base64_chars[64] = {
		'A','B','C','D','E','F','G','H','I','J','K','L','M',
		'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'a','b','c','d','e','f','g','h','i','j','k','l','m',
		'n','o','p','q','r','s','t','u','v','w','x','y','z',
		'0','1','2','3','4','5','6','7','8','9','+','/'
	};

	std::string Base64::encode(const void* data, size_t size)
	{
		const uint8_t* src = static_cast<const uint8_t*>(data);
		std::string result;
		result.reserve(((size + 2) / 3) * 4);

		size_t i = 0;
		for (; i + 2 < size; i += 3)
		{
			uint32_t v = (uint32_t(src[i]) << 16) | (uint32_t(src[i + 1]) << 8) | src[i + 2];
			result.push_back(base64_chars[(v >> 18) & 63]);
			result.push_back(base64_chars[(v >> 12) & 63]);
			result.push_back(base64_chars[(v >> 6) & 63]);
			result.push_back(base64_chars[v & 63]);
		}
		if (i < size)
		{
			uint32_t v = uint32_t(src[i]) << 16;
			if (i + 1 < size)
				v |= uint32_t(src[i + 1]) << 8;
			result.push_back(base64_chars[(v >> 18) & 63]);
			result.push_back(base64_chars[(v >> 12) & 63]);
			result.push_back(i + 1 < size ? base64_chars[(v >> 6) & 63] : '=');
			result.push_back('=');
		}
		return result;
	}

	std::string Base64::encode(const std::string& data)
	{
		return encode(data.data(), data.size());
	}

	std::string Base64::encode(const std::vector<uint8_t>& data)
	{
		return encode(data.data(), data.size());
	}

	std::vector<uint8_t> Base64::decode(const std::string& input)
	{
		std::vector<uint8_t> result;
		result.reserve(((input.size() + 3) / 4) * 3);

		uint32_t acc = 0;
		int bits = 0;
		for (unsigned char c : input)
		{
			const uint8_t v = char_to_base64[c];
			if (v == 64)
				continue;
			acc = (acc << 6) | v;
			bits += 6;
			if (bits >= 8)
			{
				bits -= 8;
				result.push_back(uint8_t(acc >> bits));
			}
		}
		return result;
	}

	std::string Base64::decodeText(const std::string& input)
	{
		const std::vector<uint8_t> bytes = decode(input);
		return std::string(bytes.begin(), bytes.end());
	}
}
