#pragma once

#include <string>

class Environment
{
public:
	static std::string getVariable(const std::string& name);
	static void setVariable(const std::string& name, const std::string& value);
};
