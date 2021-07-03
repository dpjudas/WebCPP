#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

class ZipReader
{
public:
	static std::unique_ptr<ZipReader> open(std::string filename);

	virtual ~ZipReader() = default;
	virtual std::vector<uint8_t> read_all_bytes(const std::string& filename) = 0;
	virtual std::string read_all_text(const std::string& filename) = 0;
};
