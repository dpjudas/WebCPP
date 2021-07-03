#pragma once

#include "Service.h"
#include "Webserver.h"

class WebsiteService : public Service
{
public:
	WebsiteService(std::vector<std::string> args);
	~WebsiteService();

private:
	std::unique_ptr<Webserver> webserver;
};
