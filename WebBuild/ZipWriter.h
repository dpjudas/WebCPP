#pragma once

#include <memory>
#include <string>

class ZipWriter
{
public:
	static std::unique_ptr<ZipWriter> create(std::string filename);

	virtual ~ZipWriter() = default;
	virtual void add_file(const std::string& filename, const void* data, size_t size) = 0;
	virtual void finalize_archive() = 0;
};
