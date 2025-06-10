
#include "Precomp.h"
#include "ActionModule.h"

std::shared_ptr<ActionModule> ActionModule::create(std::string baseRoute, WebRequestVerb verb, std::function<void(WebContext* context)> handler)
{
	return std::make_shared<ActionModule>(std::move(baseRoute), verb, std::move(handler));
}

ActionModule::ActionModule(std::string baseRoute, WebRequestVerb verb, std::function<void(WebContext* context)> handler) : WebModule(std::move(baseRoute)), verb(verb), handler(std::move(handler))
{
}

void ActionModule::process(WebContext* context)
{
	if (verb == WebRequestVerb::any || context->request.verb == verb)
		handler(context);
}
