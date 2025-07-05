#pragma once

#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "JsonValue.h"

namespace web
{
	class JSCallback;

	enum class OAuthStatus
	{
		unauthenticated,
		authenticated,
		loginError
	};

	class NavigationRouter
	{
	public:
		virtual ~NavigationRouter() = default;
		virtual void onNavigate() = 0;
	};

	class Navigation
	{
	public:
		static void init(std::unique_ptr<NavigationRouter> router);
		static NavigationRouter* getRouter();

		static OAuthStatus getOAuthStatus();
		static std::string getLoginError();
		static JsonValue getJwt();
		static std::string getAccessToken();

		static std::vector<std::string> getPathParts();
		static bool matchesPath(const std::vector<std::string>& parts);

		static void navigateTo(std::string url, bool replace = false);

		static void beginLogin(std::string loginUrl);
		static void endLogin(OAuthStatus status, const std::string& accessToken = {}, const std::string& loginError = {});

		static void pushState(std::string pageTitle, std::vector<std::string> pathParts);
		static void replaceState(std::string pageTitle, std::vector<std::string> pathParts);

		static std::string pagePathToURI(const std::vector<std::string>& pathparts);

		static std::string encodeURIComponent(const std::string& component);
		static std::string decodeURIComponent(const std::string& component);
	};
}
