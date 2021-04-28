
#include "Navigation.h"
#include "JSValue.h"
#include "JSCallback.h"
#include "Event.h"

namespace
{
	std::unique_ptr<JSCallback> onPopStateCallback;
	std::vector<std::string> locationPathParts;
	OAuthStatus authStatus = OAuthStatus::unauthenticated;
	std::string authError;
	JSValue jwt = JSValue::undefined();
	std::string accessToken;
}

void Navigation::init(std::function<void()> onPopState)
{
	onPopStateCallback = std::make_unique<JSCallback>([=](JSValue args)
	{
		Event event(args[0]);
		locationPathParts = vecFromJSArray<std::string>(event.handle["state"]["pathparts"]);
		if (onPopState)
			onPopState();
		return JSValue::undefined();
	});

	JSValue::global("window").call<void>("addEventListener", std::string("popstate"), onPopStateCallback->getHandler());

	JSValue navInfo = JSValue::global("OAuth").call<JSValue>("checkAccessToken");
	locationPathParts = vecFromJSArray<std::string>(navInfo["pathparts"]);
	if (!navInfo["error"].isUndefined())
	{
		authStatus = OAuthStatus::loginError;
		authError = navInfo["error"].as<std::string>();
		printf("auth error! %s\n", authError.c_str());
	}
	else if (!navInfo["access_token"].isUndefined())
	{
		authStatus = OAuthStatus::authenticated;
		accessToken = navInfo["access_token"].as<std::string>();
		jwt = navInfo["jwt"];
	}
	else
	{
		authStatus = OAuthStatus::unauthenticated;
	}
	replaceState(JSValue::global("document")["title"].as<std::string>(), locationPathParts);
}

void Navigation::login(std::string oauthUrl)
{
	JSValue::global("OAuth").call<JSValue>("login", oauthUrl);
}

OAuthStatus Navigation::getOAuthStatus()
{
	return authStatus;
}

std::string Navigation::getLoginError()
{
	return authError;
}

JSValue Navigation::getJwt()
{
	return jwt;
}

std::string Navigation::getAccessToken()
{
	return accessToken;
}

std::vector<std::string> Navigation::getPathParts()
{
	return locationPathParts;
}

void Navigation::pushState(std::string pageTitle, std::vector<std::string> pathParts)
{
	JSValue state = JSValue::object();
	state.set("pathparts", JSValue::array(pathParts));
	JSValue::global("history").call<void>("pushState", state, pageTitle, pagePathToURI(pathParts));
	locationPathParts = std::move(pathParts);
}

void Navigation::replaceState(std::string pageTitle, std::vector<std::string> pathParts)
{
	JSValue state = JSValue::object();
	state.set("pathparts", JSValue::array(pathParts));
	JSValue::global("history").call<void>("replaceState", state, pageTitle, pagePathToURI(pathParts));
	locationPathParts = std::move(pathParts);
}

void Navigation::navigateTo(std::string url)
{
	JSValue::global("window").set("location", url);
}

std::string Navigation::pagePathToURI(const std::vector<std::string>& pathparts)
{
	std::string uri = "/";
	for (size_t i = 0; i < pathparts.size(); i++)
	{
		if (!uri.empty() && uri.back() != '/')
			uri.push_back('/');
		uri += JSValue::global("encodeURIComponent")(pathparts[i]).as<std::string>();
	}
	return uri;
}
