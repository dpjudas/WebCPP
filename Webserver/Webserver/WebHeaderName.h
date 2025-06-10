#pragma once

class WebHeaderName
{
public:
	WebHeaderName() = default;
	WebHeaderName(const char* str) : str(str) { }
	WebHeaderName(std::string str) : str(std::move(str)) { }

	bool operator==(const char* other) const { return compare(other) == 0; }
	bool operator==(const std::string& other) const { return compare(other) == 0; }
	bool operator!=(const char* other) const { return compare(other) != 0; }
	bool operator!=(const std::string& other) const { return compare(other) != 0; }

	bool operator==(const WebHeaderName& other) const { return compare(other) == 0; }
	bool operator!=(const WebHeaderName& other) const { return compare(other) != 0; }
	bool operator<(const WebHeaderName& other) const { return compare(other) < 0; }
	bool operator>(const WebHeaderName& other) const { return compare(other) > 0; }
	bool operator<=(const WebHeaderName& other) const { return compare(other) <= 0; }
	bool operator>=(const WebHeaderName& other) const { return compare(other) >= 0; }

	int compare(const char* b) const;
	int compare(const std::string& b) const;
	int compare(const WebHeaderName& b) const;

	const std::string& toString() const { return str; }

private:
	std::string str;
};
