#pragma once

/// Unicode byte order mark (BOM) types
enum class ByteOrderMark
{
	none,
	utf32_be,
	utf32_le,
	utf16_be,
	utf16_le,
	utf8
};

/// Text string functions
class Text
{
public:
	/// Split text into a list
	static std::vector<std::string> split(const std::string &text, const std::string &split_string, bool skip_empty = true);

	/// Trim whitespace from front and back
	static std::string trim(const std::string &text);

	/// \brief Removes trailing zeros from floating point numbers
	///
	/// If the input string is "42.21000", then this function returns "42.21".
	/// If the string is "42.00000", then it returns "42".
	static std::string remove_trailing_zeros(std::string text);

	/// Returns true if equal when doing a character case-insensitive comparison
	static bool equal_caseless(const std::string &a, const std::string &b);

	/// Returns true if less when doing a character case-insensitive comparison
	static bool less_caseless(const std::string &a, const std::string &b);

	/// Convert text to uppercase
	static std::string to_upper(const std::string &s);

	/// Convert text to lowercase
	static std::string to_lower(const std::string &s);

	/// Convert UCS-2 code point to UTF-8
	static std::string from_wchar(wchar_t value);

	/// Convert Unicode code point to UTF-8
	static std::string from_utf32(unsigned int value);

	/// Convert from UTF-16 to UTF-8
	static std::string from_utf16(const std::wstring &s);

	/// Convert from UTF-8 to UTF-16
	static std::wstring to_utf16(const std::string &s);

	/// Formats a number
	static std::string to_string(unsigned char value, int base = 10, bool uppercase = false) { return to_string((unsigned int)value, base, uppercase); }
	static std::string to_string(unsigned short value, int base = 10, bool uppercase = false) { return to_string((unsigned int)value, base, uppercase); }
	static std::string to_string(unsigned int value, int base = 10, bool uppercase = false);
	static std::string to_string(unsigned long long value);
	static std::string to_string(char value, int base = 10, bool uppercase = false) { return to_string((int)value, base, uppercase); }
	static std::string to_string(short value, int base = 10, bool uppercase = false) { return to_string((int)value, base, uppercase); }
	static std::string to_string(int value, int base = 10, bool uppercase = false);
	static std::string to_string(long long value);
	static std::string to_string(float value, int digits = 6, bool remove_trailing_zeros = true);
	static std::string to_string(double value, int digits = 6, bool remove_trailing_zeros = true);
	static std::string to_string(bool value);

	/// Parse a number
	static unsigned int parse_uint32(const std::string &value, int base = 10);
	static int parse_int32(const std::string &value, int base = 10);
	static unsigned long long parse_uint64(const std::string &value, int base = 10);
	static long long parse_int64(const std::string &value, int base = 10);
	static float parse_float(const std::string &value);
	static double parse_double(const std::string &value);
	static bool parse_bool(const std::string &value);

	/// Finds the number of characters in a UTF-8 string
	//static std::string::size_type char_length(const std::string &str);

	/// Searches for a byte order mark
	static ByteOrderMark detect_bom(const void *data, std::string::size_type length);
};
