#pragma once

#include <string>

class WebContext;

class WebModule
{
public:
	WebModule(std::string baseRoute) : baseRoute(std::move(baseRoute)) {}

	virtual ~WebModule() = default;
	virtual void process(WebContext* context) = 0;

	std::string baseRoute;
};
