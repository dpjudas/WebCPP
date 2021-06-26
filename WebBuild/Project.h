#pragma once

#include "JsonValue.h"

class Project
{
public:
	static Project load(const std::string& filename);

	std::vector<std::string> getFilteredSources(const std::vector<std::string> extensions) const;

	std::string projectDir;
	std::string name;
	std::string type;
	std::string output;
	std::vector<std::string> sources;
	std::string wwwroot;
	std::string cssfile;
	std::string shellfile;
	std::string prejs;
	std::map<std::string, std::map<std::string, JsonValue>> configurations;
};
