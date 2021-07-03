#pragma once

#include <memory>

class Webserver
{
public:
	static std::unique_ptr<Webserver> create();

	virtual ~Webserver() = default;
};
