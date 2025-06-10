
#pragma once

#include "IOData/IODevice.h"
#include "IOData/DataBuffer.h"

struct ZipFileEntry
{
	uint32_t fileIndex = 0;
	uint64_t centralDirOffset = 0;
	uint16_t versionMadeBy = 0;
	uint16_t versionNeeded = 0;
	uint16_t bitFlag = 0;
	uint16_t method = 0;
	time_t time = 0;
	uint32_t fileCrc32 = 0;
	uint64_t compSize = 0;
	uint64_t uncompSize = 0;
	uint16_t internalAttr = 0;
	uint32_t externalAttr = 0;
	uint64_t localHeaderOffset = 0;
	uint32_t commentSize = 0;
	bool isDirectory = false;
	bool isEncrypted = false;
	bool isSupported = false;
	std::string filename;
	std::string comment;
};

class ZipReader
{
public:
	static std::unique_ptr<ZipReader> open(std::shared_ptr<IODevice> input);

	virtual ~ZipReader() = default;
	virtual const std::vector<ZipFileEntry>& getFiles() = 0;
	virtual std::shared_ptr<DataBuffer> readAllBytes(uint32_t file_index) = 0;
	virtual std::shared_ptr<DataBuffer> readAllBytes(const std::string& filename) = 0;
	virtual std::string readAllText(uint32_t file_index) = 0;
	virtual std::string readAllText(const std::string& filename) = 0;
	virtual uint32_t getCrc32(uint32_t file_index) = 0;
	virtual uint32_t getCrc32(const std::string& filename) = 0;
};
