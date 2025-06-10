#pragma once

#include "WebContext.h"
#include "WebModule.h"

class DataBuffer;

class FileModuleReader
{
public:
	virtual ~FileModuleReader() = default;
	virtual uint32_t getCrc32(const std::string& filename) = 0;
	virtual std::shared_ptr<DataBuffer> readAllBytes(const std::string& filename) = 0;
};

class FileModule : public WebModule
{
public:
	FileModule(std::string baseRoute, std::unique_ptr<FileModuleReader> reader, std::string singlePageApplicationFilename);
	~FileModule();

	static std::shared_ptr<FileModule> createZip(std::string baseRoute, const std::string& filename, std::string singlePageApplicationFilename = {});
	static std::shared_ptr<FileModule> createFolder(std::string baseRoute, const std::string& path, std::string singlePageApplicationFilename = {});

	void process(WebContext* context) override;

private:
	static void validatePath(const std::string& filename);

	std::unique_ptr<FileModuleReader> reader;
	std::string singlePageApplicationFilename;
	std::map<std::string, std::string> extensionMimeTypes;
};
