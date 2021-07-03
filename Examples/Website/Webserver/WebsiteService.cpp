
#include "WebsiteService.h"
#include <iostream>

WebsiteService::WebsiteService(std::vector<std::string> args)
{
	std::cout << "Starting website" << std::endl;
	webserver = Webserver::create();
	std::cout << "Website started" << std::endl;
	// webserver->run();
}

WebsiteService::~WebsiteService()
{
	std::cout << "Website stopping" << std::endl;
}
