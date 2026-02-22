#pragma once

#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "JsonValue.h"

namespace web
{
	class JSCallback;
	class View;

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
		virtual void onNavigate();

		template<typename T>
		void addRoute(const std::vector<std::string>& pathparts, bool authRequired = false) { addRoute(pathparts, []() { return std::make_shared<T>(); }, authRequired); }
		void addRoute(const std::vector<std::string>& pathparts, std::function<std::shared_ptr<View>()> createPage, bool authRequired = false);

		const std::string& getLoginUrl() const { return loginUrl; }
		void setLoginUrl(const std::string& url);

		template<typename T>
		void setLoginErrorPage() { setLoginErrorPage([]() { return std::make_shared<T>(); }); }
		void setLoginErrorPage(std::function<std::shared_ptr<View>()> createPage);

		template<typename T>
		void setNotFoundPage() { setNotFoundPage([]() { return std::make_shared<T>(); }); }
		void setNotFoundPage(std::function<std::shared_ptr<View>()> createPage);

	private:
		struct Route
		{
			std::vector<std::string> pathparts;
			std::function<std::shared_ptr<View>()> createPage;
			bool authRequired = false;
		};
		std::vector<Route> routes;
		std::string loginUrl;
		std::function<std::shared_ptr<View>()> createLoginErrorPage;
		std::function<std::shared_ptr<View>()> createNotFoundPage;
		std::shared_ptr<View> currentPage;
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
		static void logout();

		static void pushState(std::string pageTitle, std::vector<std::string> pathParts);
		static void replaceState(std::string pageTitle, std::vector<std::string> pathParts);

		static std::string pagePathToURI(const std::vector<std::string>& pathparts);

		static std::string encodeURIComponent(const std::string& component);
		static std::string decodeURIComponent(const std::string& component);
	};
}
