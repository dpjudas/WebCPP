
#pragma once

#include "include/cef_client.h"
#include "include/wrapper/cef_resource_manager.h"
#include "include/wrapper/cef_message_router.h"
#include "win32_window.h"
#include "json_value.h"
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

class PlayerController;
class JsonValue;

class BrowserClient :
	public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefRequestHandler,
	public CefResourceRequestHandler,
	public CefDragHandler
{
public:
	BrowserClient();
	~BrowserClient();

	static void Create();

	// CefClient methods:
	CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
	CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
	CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
	CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
	CefRefPtr<CefDragHandler> GetDragHandler() override { return this; }
	bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

	// CefDisplayHandler methods:
	void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;

	// CefLifeSpanHandler methods:
	void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	bool DoClose(CefRefPtr<CefBrowser> browser) override;
	void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

	// CefLoadHandler methods:
	void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) override;

	// CefRequestHandler methods:
	bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool user_gesture, bool is_redirect) override;
	CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_navigation, bool is_download, const CefString& request_initiator, bool& disable_default_handling) override;
	void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status) override;

	// CefResourceRequestHandler methods:
	cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback) override;
	CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) override;

	// CefDragHandler methods
	bool OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask) override;
	void OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const std::vector<CefDraggableRegion>& regions) override;

	void CloseAllBrowsers(bool force_close);
	bool IsClosing() const { return is_closing; }

private:
	void CreateWin32Window();
	void OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions);

	void SubclassWindow(HWND hwnd);
	void UnsubclassWindow(HWND hwnd);
	static LRESULT CALLBACK SubclassedWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	bool IsDevToolsChild(HWND hwnd) const;

	static void SetupResourceManager(CefRefPtr<CefResourceManager> resource_manager);
	static std::string GetDataURI(const std::string& data, const std::string& mime_type);

	std::unique_ptr<PlayerController> player;

	std::unique_ptr<Win32Window> win32window;
	HRGN draggable_region = 0;
	HWND devtools = 0;

	std::list<CefRefPtr<CefBrowser>> browser_list;
	bool is_closing = false;

	CefRefPtr<CefResourceManager> resource_manager;
	CefRefPtr<CefMessageRouterBrowserSide> message_router;
	scoped_ptr<CefMessageRouterBrowserSide::Handler> message_handler;

	friend class ScriptMessageHandler;

	IMPLEMENT_REFCOUNTING(BrowserClient);
};

class ScriptMessageHandler : public CefMessageRouterBrowserSide::Handler
{
public:
	ScriptMessageHandler(BrowserClient* client);
	~ScriptMessageHandler();

	// Called when cefQuery is called from javascript
	bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) override;

	BrowserClient* client;

private:
	void WorkerThreadMain();

	std::thread thread;
	std::condition_variable condvar;
	std::mutex mutex;
	bool stopflag = false;

	struct QueuedRequest
	{
		QueuedRequest(CefRefPtr<Callback> callback, JsonValue request) : callback(std::move(callback)), request(std::move(request)) { }

		CefRefPtr<Callback> callback;
		JsonValue request;
	};

	std::vector<QueuedRequest> requests;
};
