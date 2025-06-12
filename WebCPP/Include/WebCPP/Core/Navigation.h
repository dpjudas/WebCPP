#pragma once

#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "JSValue.h"

class JSCallback;

enum class OAuthStatus
{
	unauthenticated,
	authenticated,
	loginError
};

class Navigation
{
public:
	static OAuthStatus getOAuthStatus();
	static std::string getLoginError();
	static JSValue getJwt();
	static std::string getAccessToken();
	static std::vector<std::string> getPathParts();
	static bool matchesPath(const std::vector<std::string>& parts);

	static void login(std::string oauthUrl);
	static void navigateTo(std::string url);

	static void pushState(std::string pageTitle, std::vector<std::string> pathParts);
	static void replaceState(std::string pageTitle, std::vector<std::string> pathParts);

	static std::string pagePathToURI(const std::vector<std::string>& pathparts);

private:
	static void init(std::function<void()> onPopState);

	friend class DocumentBodyView;
};
