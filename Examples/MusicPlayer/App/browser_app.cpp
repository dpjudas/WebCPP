
#include "browser_app.h"
#include "browser_client.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

BrowserApp::BrowserApp()
{
}

void BrowserApp::OnContextInitialized()
{
	CEF_REQUIRE_UI_THREAD();
	// CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();
	BrowserClient::Create();
}

void BrowserApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
	// registrar->AddCustomScheme("app", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_CORS_ENABLED);
}

void BrowserApp::OnWebKitInitialized()
{
	CefMessageRouterConfig config;
	config.js_query_function = "cefQuery";
	config.js_cancel_function = "cefQueryCancel";
	message_router = CefMessageRouterRendererSide::Create(config);
}

void BrowserApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	message_router->OnContextCreated(browser, frame, context);
}

void BrowserApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	message_router->OnContextReleased(browser, frame, context);
}

bool BrowserApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	return message_router->OnProcessMessageReceived(browser, frame, source_process, message);
}
