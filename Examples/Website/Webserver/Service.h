#pragma once

#include <functional>
#include <string>
#include <vector>

class Service
{
public:
	template<typename T>
	static int main(int argc, char** argv, const std::string& serviceName)
	{
		return main(argc, argv, serviceName, [](std::vector<std::string> args) { return std::make_unique<T>(std::move(args)); });
	}

	static int main(int argc, char** argv, const std::string& serviceName, std::function<std::unique_ptr<Service>(std::vector<std::string> args)> serviceStart);

	virtual ~Service() = default;
	virtual void reload() { }
};
