
#include <Webserver/Webserver.h>
#include <Webserver/FileModule.h>
#include <Webserver/ActionModule.h>
#include <Webserver/JsonValue.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#ifdef WIN32
#include <Windows.h>
#endif

std::string getExePath();

void apiTestRequest(web::WebContext* context)
{
	web::JsonValue request = context->getJsonRequest();
	web::JsonValue response = web::JsonValue::object();
	response["pong"] = request["ping"];
	context->setJsonResponse(response);
}

int main(int argc, char* argv[])
{
	std::unique_ptr<web::Webserver> webserver = web::Webserver::create(web::WebserverType::tcpSocket);
	webserver->addModule(web::ActionModule::create("/api/test", web::WebRequestVerb::post, apiTestRequest));
	webserver->addModule(web::FileModule::createZip("/", getExePath() + "ExampleWebsite.webpkg", "ExampleWebsite.html"));
	webserver->start();

	std::cout << "Listening on http//localhost:8080" << std::endl;
	std::cin.get();
	return 0;
}

std::string getExePath()
{
#ifdef WIN32
	std::vector<char> buffer(1024);
	if (!GetModuleFileNameA(0, buffer.data(), 1024))
		throw std::runtime_error("getExePath failed!");
	buffer.back() = 0;
	std::string exeFilename = buffer.data();
	while (!exeFilename.empty() && !(exeFilename.back() == '\\' || exeFilename.back() == '/'))
		exeFilename.pop_back();
	return exeFilename;
#else
	return {};
#endif
}
