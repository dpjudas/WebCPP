
#include "Precomp.h"
#include "Webserver/WebController.h"
#include "Webserver/WebContext.h"

namespace web
{
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
				context->setJsonResponse(it->second(context));
			}
			else
			{
				context->setTextResponse("Endpoint not found", "text/plain", 404, "Not Found");
			}
		}
		catch (const WebControllerException& e)
		{
			auto response = JsonValue::object();
			response["Error"] = JsonValue::string(e.getMessage());
			context->setJsonResponse(response, e.getStatusCode(), e.getStatusText());
		}
		catch (const std::exception& e)
		{
			auto response = JsonValue::object();
			response["Error"] = JsonValue::string(e.what());
			context->setJsonResponse(response, 500, "Internal Server Error");
		}
	}
}
