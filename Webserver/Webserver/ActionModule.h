#pragma once

#include "WebModule.h"
#include "WebContext.h"

class ActionModule : public WebModule
{
public:
	static std::shared_ptr<ActionModule> create(std::string baseRoute, WebRequestVerb verb, std::function<void(WebContext* context)> handler);

	ActionModule(std::string baseRoute, WebRequestVerb verb, std::function<void(WebContext* context)> handler);
	void process(WebContext* context) override;

private:
	WebRequestVerb verb = WebRequestVerb::any;
	std::function<void(WebContext* context)> handler;
};
