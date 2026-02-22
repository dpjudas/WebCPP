
#include "WebCPP/Core/Navigation.h"
#include "WebCPP/Core/JSValue.h"
#include "WebCPP/Core/JSCallback.h"
#include "WebCPP/Core/Event.h"
#include "WebCPP/Core/Storage.h"
#include "WebCPP/Core/HtmlDocument.h"
#include "WebCPP/Core/View.h"
#include "WebCPP/Core/DateTime.h"
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
			size_t nextpos = pathname.find('/', pos);
			if (nextpos == std::string::npos)
				nextpos = pathname.size();
			parts.push_back(Navigation::decodeURIComponent(pathname.substr(pos, nextpos - pos)));
			pos = nextpos + 1;
		}
		return parts;
	}

	namespace // base64 decoder that we want to keep completely private
	{
		static uint8_t char_to_base64[256] =
		{
			 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0, 62,  0,  0,  0, 63,
			52, 53, 54, 55, 56, 57, 58, 59,  60, 61,  0,  0,  0,  0,  0,  0,
			 0,  0,  1,  2,  3,  4,  5,  6,   7,  8,  9, 10, 11, 12, 13, 14,
			15, 16, 17, 18, 19, 20, 21, 22,  23, 24, 25,  0,  0,  0,  0,  0,
			 0, 26, 27, 28, 29, 30, 31, 32,  33, 34, 35, 36, 37, 38, 39, 40,
			41, 42, 43, 44, 45, 46, 47, 48,  49, 50, 51,  0,  0,  0,  0,  0,

			 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0
		};

		class Base64Decoder
		{
		public:
			std::vector<uint8_t> result;

			unsigned char chunk[4] = {};
			size_t chunk_filled = 0;

			static void decode(unsigned char* output, const unsigned char* input, size_t size_input)
			{
				int i, o;
				for (i = 0, o = 0; i < size_input; i += 4, o += 3)
				{
					unsigned int v1 = char_to_base64[input[i + 0]];
					unsigned int v2 = char_to_base64[input[i + 1]];
					unsigned int v3 = char_to_base64[input[i + 2]];
					unsigned int v4 = char_to_base64[input[i + 3]];
					unsigned int value = (v1 << 18) + (v2 << 12) + (v3 << 6) + v4;

					output[o + 0] = (value >> 16) & 255;
					output[o + 1] = (value >> 8) & 255;
					output[o + 2] = value & 255;
				}
			}

			void reset()
			{
				result.resize(0);
				chunk_filled = 0;
			}

			void feed(const void* _data, size_t size, bool append_result)
			{
				size_t pos = 0;
				const unsigned char* data = (const unsigned char*)_data;

				if (!append_result)
					result.resize(0);

				// Handle any left-over data from last encode:

				if (chunk_filled > 0)
				{
					size_t needed = 4 - chunk_filled;
					if (size >= needed)
					{
						// Finish left-over data block:

						memcpy(chunk + chunk_filled, data, needed);
						size_t out_pos = result.size();
						result.resize(out_pos + 3);
						decode(result.data() + out_pos, chunk, 4);
						pos += needed;
						chunk_filled = 0;

						// Shorten result if we got an end of base64 data marker:

						if (chunk[2] == '=')
							result.resize(result.size() - 2);
						else if (chunk[3] == '=')
							result.resize(result.size() - 1);
					}
					else
					{
						std::memcpy(chunk + chunk_filled, data, size);
						chunk_filled += size;
						return;
					}
				}

				// Base64 encode what's available to us now:

				size_t blocks = (size - pos) / 4;
				size_t out_pos = result.size();
				result.resize(out_pos + blocks * 3);
				decode((unsigned char*)result.data() + out_pos, data + pos, blocks * 4);
				pos += blocks * 4;

				// Shorten result if we got an end of base64 data marker:

				if (blocks > 0)
				{
					if (data[pos - 2] == '=')
						result.resize(result.size() - 2);
					else if (data[pos - 1] == '=')
						result.resize(result.size() - 1);
				}

				// Save data for last incomplete block:

				size_t leftover = size - pos;
				if (leftover > 4)
					throw std::runtime_error("Base64 decoder is broken!");
				chunk_filled = leftover;
				std::memcpy(chunk, data + pos, leftover);
			}
		};

		static std::string base64Decode(const std::string& input)
		{
			Base64Decoder decoder;
			decoder.feed(input.data(), input.size(), false);
			return std::string((const char*)decoder.result.data(), decoder.result.size());
		}

		static std::string base64UrlToBase64(std::string base64url)
		{
			for (char& c : base64url)
			{
				if (c == '-')
					c = '+';
				else if (c == '_')
					c = '/';
			}

			if (base64url.size() % 4 == 2)
				base64url += "==";
			else if (base64url.size() % 4 == 3)
				base64url += "=";
			return base64url;
		}

		static JsonValue extractJwt(const std::string& token)
		{
			size_t dot1 = token.find('.');
			if (dot1 == std::string::npos)
				return JsonValue::undefined();
			size_t dot2 = token.find('.', dot1 + 1);
			if (dot2 == std::string::npos)
				return JsonValue::undefined();
			try
			{
				return JsonValue::parse(base64Decode(base64UrlToBase64(token.substr(dot1 + 1, dot2 - dot1 - 1))));
			}
			catch (...)
			{
				return JsonValue::undefined();
			}
		}
	}

	void Navigation::init(std::unique_ptr<NavigationRouter> initRouter)
	{
		router = std::move(initRouter);

		// Are we still authenticated?
		std::string token = SessionStorage::getString("webcpp.token");
		JsonValue savedjwt = extractJwt(token);
		if (!savedjwt.is_undefined() && savedjwt.properties().find("exp") != savedjwt.properties().end())
		{
			int64_t expireTime = (int64_t)savedjwt["exp"].to_number();
			if (DateTime::now().getTime() + 30 > expireTime)
			{
				// Yes we are. Mark us as authenticated.
				// Note that this doesn't guarantee the server will still accept the access token, but normally it would.
				authStatus = OAuthStatus::authenticated;
				accessToken = std::move(token);
				authError = {};
				jwt = savedjwt;
			}
		}
		if (authStatus != OAuthStatus::authenticated)
			SessionStorage::removeItem("webcpp.token");

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
		jwt = extractJwt(accessToken);

		if (authStatus == OAuthStatus::authenticated && !jwt.is_undefined() && !accessToken.empty())
		{
			SessionStorage::setString("webcpp.token", accessToken);
		}

		std::string url = SessionStorage::getString("webcpp.loginFromUrl");
		if (!url.empty())
		{
			SessionStorage::removeItem("webcpp.loginFromUrl");
			navigateTo(url, true);
		}
	}

	void Navigation::logout()
	{
		SessionStorage::removeItem("webcpp.token");
		authStatus = OAuthStatus::unauthenticated;
		accessToken = {};
		authError = {};
		jwt = JsonValue::undefined();
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

	////////////////////////////////////////////////////////////////////////////

	void NavigationRouter::addRoute(const std::vector<std::string>& pathparts, std::function<std::shared_ptr<View>()> createPage, bool authRequired)
	{
		Route route;
		route.pathparts = pathparts;
		route.createPage = createPage;
		route.authRequired = authRequired;
		routes.push_back(std::move(route));
	}

	void NavigationRouter::setLoginUrl(const std::string& url)
	{
		loginUrl = url;
	}

	void NavigationRouter::setLoginErrorPage(std::function<std::shared_ptr<View>()> createPage)
	{
		createLoginErrorPage = std::move(createPage);
	}

	void NavigationRouter::setNotFoundPage(std::function<std::shared_ptr<View>()> createPage)
	{
		createNotFoundPage = std::move(createPage);
	}

	void NavigationRouter::onNavigate()
	{
		if (currentPage)
		{
			currentPage->detach();
			currentPage.reset();
		}

		if (Navigation::getOAuthStatus() == OAuthStatus::loginError)
		{
			if (createLoginErrorPage)
				currentPage = createLoginErrorPage();
		}
		else
		{
			for (const Route& route : routes)
			{
				if (Navigation::matchesPath(route.pathparts))
				{
					if (route.authRequired && Navigation::getOAuthStatus() == OAuthStatus::unauthenticated)
					{
						Navigation::beginLogin(loginUrl);
						return;
					}
					else if (route.createPage)
					{
						currentPage = route.createPage();
						if (currentPage)
							break;
					}
				}
			}
		}

		if (!currentPage && createNotFoundPage)
			currentPage = createNotFoundPage();

		if (currentPage)
		{
			HtmlDocument::body()->addView(currentPage);
			currentPage->applyDefaultFocus();
		}
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
