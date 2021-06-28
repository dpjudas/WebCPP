
#pragma once

#include "include/cef_app.h"
#include "include/wrapper/cef_message_router.h"

class BrowserApp : public CefApp, public CefBrowserProcessHandler, public CefRenderProcessHandler
{
public:
	BrowserApp();

	// CefApp methods:
	CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
	CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }

	// CefBrowserProcessHandler methods:
	void OnContextInitialized() override;
	void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

	// CefRenderProcessHandler methods:
	void OnWebKitInitialized() override;
	void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
	void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
	bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

private:
	CefRefPtr<CefMessageRouterRendererSide> message_router;

	IMPLEMENT_REFCOUNTING(BrowserApp);
};
