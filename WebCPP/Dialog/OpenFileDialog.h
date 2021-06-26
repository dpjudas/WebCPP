#pragma once

#include "WebCPP/Core/View.h"
#include "WebCPP/Core/JSValue.h"

class OpenFileEntry
{
public:
	OpenFileEntry(JSValue file);

	std::string name() const;
	int64_t size() const;
	std::string mimeType() const;
	int64_t lastModified() const;

	void readAllText(std::function<void(std::string text)> onResult);
	void readAllBytes(std::function<void(std::vector<uint8_t> data)> onResult);

	JSValue handle;
};

class OpenFileDialog
{
public:
	static void show(const std::string& filter, bool multiple, std::function<void(std::vector<OpenFileEntry> files)> selectedCallback);
};
