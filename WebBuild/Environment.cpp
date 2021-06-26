
#include "Precomp.h"
#include "Environment.h"
#include <stdexcept>

std::string Environment::getVariable(const std::string& name)
{
	size_t needed = 0;
	errno_t result = getenv_s(&needed, nullptr, 0, name.c_str());
	if (result != 0 && result != ERANGE)
		throw std::runtime_error("Could not get environment variable " + name + "!");

	if (needed == 0)
		return {};

	std::string buffer;
	buffer.resize(needed);
	result = getenv_s(&needed, &buffer[0], buffer.size(), name.c_str());
	if (result != 0)
		throw std::runtime_error("Could not get environment variable " + name + "!");
	if (buffer.back() == 0)
		buffer.pop_back();
	return buffer;
}

void Environment::setVariable(const std::string& name, const std::string& value)
{
	errno_t result = _putenv_s(name.c_str(), value.c_str());
	if (result != 0)
		throw std::runtime_error("Could not set environment variable " + name + "!");
}
