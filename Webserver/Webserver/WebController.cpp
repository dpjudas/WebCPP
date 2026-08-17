
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
				it->second(context);
		}
		catch (const WebControllerException& e)
		{
			auto response = JsonValue::object();
			response["Error"] = JsonValue::string(e.getMessage());
			context->setJsonResponse(response, e.getStatusCode(), e.getStatusText());
			if (e.getRetryAfterSeconds() > 0)
				context->response.headers["Retry-After"] = std::to_string(e.getRetryAfterSeconds());
		}
		catch (const std::exception& e)
		{
			auto response = JsonValue::object();
			response["Error"] = JsonValue::string(e.what());
			context->setJsonResponse(response, 500, "Internal Server Error");
		}
	}
}
