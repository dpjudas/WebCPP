
#include "WebCPP/Core/Navigation.h"
#include "WebCPP/Core/JSValue.h"
#include "WebCPP/Core/JSCallback.h"
#include "WebCPP/Core/Event.h"
#include "WebCPP/Core/Storage.h"
#include <regex>
#include <emscripten/emscripten.h>

namespace web
{
	namespace
	{
		std::unique_ptr<JSCallback> onPopStateCallback;
		std::vector<std::string> locationPathParts;
		OAuthStatus authStatus = OAuthStatus::unauthenticated;
		std::string authError;
		JsonValue jwt = JsonValue::undefined();
		std::string accessToken;
		std::unique_ptr<NavigationRouter> router;
	}

	static std::vector<std::string> pathPartsFromPathname(const std::string& pathname)
	{
		std::vector<std::string> parts;
		size_t pos = 1;
		while (pos < pathname.size())
		{
			size_t nextpos = pathname.find(pos, '/');
			if (nextpos == std::string::npos)
				nextpos = pathname.size();
			parts.push_back(Navigation::decodeURIComponent(pathname.substr(pos, nextpos - pos)));
			pos = nextpos + 1;
		}
		return parts;
	}

	void Navigation::init(std::unique_ptr<NavigationRouter> initRouter)
	{
		router = std::move(initRouter);

		onPopStateCallback = std::make_unique<JSCallback>([=](JSValue args)
			{
				Event event(args[0]);
				locationPathParts = vecFromJSArray<std::string>(event.handle["state"]["pathparts"]);
				if (router)
					router->onNavigate();
				return JSValue::undefined();
			});
		JSValue::global("window").call<void>("addEventListener", std::string("popstate"), onPopStateCallback->getHandler());

		locationPathParts = pathPartsFromPathname(JSValue::global("document")["location"]["pathname"].as<std::string>());
		// replaceState(JSValue::global("document")["title"].as<std::string>(), locationPathParts);

		router->onNavigate();
	}

	NavigationRouter* Navigation::getRouter()
	{
		return router.get();
	}

	void Navigation::beginLogin(std::string url)
	{
		SessionStorage::setString("webcpp.loginFromUrl", JSValue::global("document")["location"]["href"].as<std::string>());
		navigateTo(url, true);
	}

	void Navigation::endLogin(OAuthStatus newStatus, const std::string& newAccessToken, const std::string& newLoginError)
	{
		authStatus = newStatus;
		accessToken = newAccessToken;
		authError = newLoginError;

		std::string url = SessionStorage::getString("webcpp.loginFromUrl");
		if (!url.empty())
		{
			SessionStorage::removeItem("webcpp.loginFromUrl");
			navigateTo(url, true);
		}
	}

	OAuthStatus Navigation::getOAuthStatus()
	{
		return authStatus;
	}

	std::string Navigation::getLoginError()
	{
		return authError;
	}

	JsonValue Navigation::getJwt()
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

	void Navigation::navigateTo(std::string url, bool replace)
	{
		std::string pageOrigin = JSValue::global("window")["location"]["origin"].as<std::string>();
		std::string urlOrigin;

		JSValue parsedUrl = JSValue::global("URL").call<JSValue>("parse", url, JSValue::global("window")["location"]["href"]);
		if (!parsedUrl.isNull())
			urlOrigin = parsedUrl["origin"].as<std::string>();

		if (urlOrigin != pageOrigin || !router) // Navigating to external page
		{
			if (!replace)
				JSValue::global("window").set("location", url);
			else
				JSValue::global("window")["location"].call<void>("replace", url);
		}
		else
		{
			std::string title = JSValue::global("document")["title"].as<std::string>();
			auto pathParts = pathPartsFromPathname(parsedUrl["pathname"].as<std::string>());
			if (!replace)
				pushState(title, pathParts);
			else
				replaceState(title, pathParts);
			router->onNavigate();
		}
	}

	std::string Navigation::pagePathToURI(const std::vector<std::string>& pathparts)
	{
		std::string uri = "/";
		for (size_t i = 0; i < pathparts.size(); i++)
		{
			if (!uri.empty() && uri.back() != '/')
				uri.push_back('/');
			uri += encodeURIComponent(pathparts[i]);
		}
		return uri;
	}

	std::string Navigation::encodeURIComponent(const std::string& component)
	{
		return JSValue::global("encodeURIComponent")(component).as<std::string>();
	}

	std::string Navigation::decodeURIComponent(const std::string& component)
	{
		return JSValue::global("decodeURIComponent")(component).as<std::string>();
	}

	bool Navigation::matchesPath(const std::vector<std::string>& parts)
	{
		static std::regex escapeString("[.*+?^${}()|[\\]\\\\]");

		size_t index = 0;
		while (index < parts.size())
		{
			const std::string& text = parts[index];
			if (text == "**")
			{
				return true;
			}

			if (index >= locationPathParts.size())
				return false;

			if (text != locationPathParts[index] && text != "*")
			{
				std::regex searchregex("^" + std::regex_replace(text, escapeString, "\\$&") + "$", std::regex::ECMAScript | std::regex::icase);
				if (!std::regex_search(locationPathParts[index], searchregex))
					return false;
			}
			index++;
		}

		// We reached the end and everything matched
		if (index >= locationPathParts.size())
			return true;

		// We have one part left and it's empty (URL ended the path with a slash)
		if (index + 1 == locationPathParts.size() && locationPathParts[index].empty())
			return true;

		return false;
	}
}

#if 0
static struct JSCode { JSCode() { emscripten_run_script(R"jscode(

	OAuth = function () {
		function parseJwt(token) {
			var base64Url = token.split('.')[1];
			var base64 = base64Url.replace(/-/g, '+').replace(/_/g, '/');
			var jsonPayload = decodeURIComponent(atob(base64).split('').map(function (c) {
				return '%' + ('00' + c.charCodeAt(0).toString(16)).slice(-2);
			}).join(''));
			return JSON.parse(jsonPayload);
		}

		function extractToken(name) {
			var hash = window.location.hash.substr(1);
			var startIndex = hash.indexOf(name + "=");
			if (startIndex >= 0) {
				startIndex += name.length + 1;
				var endIndex = hash.indexOf("&", startIndex);
				if (endIndex < 0) endIndex = hash.length;
				return hash.substr(startIndex, endIndex - startIndex);
			}
			else {
				return null;
			}
		}

		function checkAccessToken(oauthUrl) {
			var error = extractToken("error");
			if (error == null) {
				var access_token = extractToken("id_token");
				if (access_token == null) {
					var pathparts = window.location.pathname.substr(1).split('/').map(x => decodeURIComponent(x));
					return {
						pathparts: pathparts
					};
				}
				else {
					var pathname = decodeURIComponent(extractToken("state"));
					var pathparts = pathname.substr(1).split('/').map(x => decodeURIComponent(x));
					return {
						pathparts: pathparts,
						access_token: access_token,
						jwt: parseJwt(access_token)
					};
				}
			}
			else {
				var pathname = decodeURIComponent(extractToken("state"));
				var pathparts = pathname.substr(1).split('/').map(x => decodeURIComponent(x));
				var error_description = extractToken("error_description");
				return {
					pathparts: pathparts,
					error: decodeURIComponent(error),
					error_description: (error_description != null) ? decodeURIComponent(error_description) : null
				};
			}
		}

		function login(oauthUrl) {
			window.location = oauthUrl + "&state=" + encodeURIComponent(window.location.pathname);
		}

		return {
			checkAccessToken: checkAccessToken,
			login: login
		};
	}();

)jscode"); } } initJSCode;
#endif
