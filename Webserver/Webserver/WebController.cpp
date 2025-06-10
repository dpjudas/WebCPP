
#include "Precomp.h"
#include "WebController.h"
#include "WebContext.h"

WebController::WebController(std::string baseRoute) : WebModule(baseRoute)
{
}

void WebController::process(WebContext* context)
{
	if (context->request.verb != WebRequestVerb::post)
		return;

	try
	{
		auto it = bindings.find(context->requestedPath);
		if (it != bindings.end())
		{
			context->setJsonResponse(it->second(context->getJsonRequest()));
		}
		else
		{
			context->setTextResponse("Endpoint not found", "text/plain", 404, "Not Found");
		}
	}
	catch (const std::exception& e)
	{
		auto response = JsonValue::object();
		response["Error"] = JsonValue::string(e.what());
		context->setJsonResponse(response, 500, "Internal Server Error");
	}
}
