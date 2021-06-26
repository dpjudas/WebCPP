#pragma once

#include "JsonValue.h"
#include "Project.h"

class Solution
{
public:
	static Solution load(const std::string& filename);

	std::string getValue(const std::string& name, const Project* project, const std::string& configuration, const std::string& defaultValue = {}) const;

	std::string solutionDir;
	std::map<std::string, Project> projects;
	std::map<std::string, JsonValue> environment;
};
