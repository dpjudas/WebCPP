
#include "Precomp.h"
#include "Webserver.h"
#include "WebContext.h"
#include "WebModule.h"
#include "HttpSys/HttpSysWebserver.h"
#include "Socket/SocketWebserver.h"
#include <iostream>
#include <mutex>

std::unique_ptr<Webserver> Webserver::create(WebserverType type)
{
	switch (type)
	{
	default:
	case WebserverType::tcpSocket: return std::make_unique<SocketWebserver>();
	case WebserverType::httpSys: return std::make_unique<HttpSysWebserver>();
	}
}

Webserver::Webserver()
{
}

void Webserver::addModule(std::shared_ptr<WebModule> module)
{
	modules.push_back(std::move(module));
}

void Webserver::processRequest(WebContext* context)
{
	LoggerSource logSource("Webserver");

	try
	{
		// Set CORS headers that enable everything:
		context->response.headers["Access-Control-Allow-Origin"] = "*";
		if (context->request.verb == WebRequestVerb::options)
		{
			context->response.headers["Access-Control-Allow-Methods"] = "POST, GET, OPTIONS, HEAD, PATCH, PUT, DELETE";
			std::string requestHeaders = context->request.getHeader("Access-Control-Request-Headers");
			if (!requestHeaders.empty())
				context->response.headers["Access-Control-Allow-Headers"] = requestHeaders;
			context->response.headers["Access-Control-Max-Age"] = "60";
		}

		std::string path = context->request.url.getPath();
		for (auto& module : modules)
		{
			if (path.substr(0, module->baseRoute.length()) == module->baseRoute)
			{
				context->requestedPath = "/" + path.substr(module->baseRoute.length());
				module->process(context);
				if (context->handled)
					break;
			}
		}

		if (!context->handled && context->request.verb == WebRequestVerb::options) // Handle CORS preflight
		{
			context->setNoContentResponse();
		}

		if (!context->handled)
			context->setTextResponse("Not found", "text/plain", 404, "Not found");
	}
	catch (const std::exception& e)
	{
		context->setTextResponse(e.what(), "text/plain", 500, "Internal Server Error");
	}

	LogLevel level = LogLevel::info;
	if (context->response.status.code == 404)
		level = LogLevel::warning;
	else if (context->response.status.code == 500 || context->response.status.code == 403)
		level = LogLevel::error;
	logMessage(level, "%1 %2 %3", context->response.status.code, context->request.url.getPath(), context->response.getHeader("Content-Type"));
}
