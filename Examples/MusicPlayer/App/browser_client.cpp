
#include "browser_client.h"
#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/cef_parser.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "player_controller.h"
#include "file.h"
#include <string>

BrowserClient::BrowserClient()
{
	resource_manager = new CefResourceManager();
	SetupResourceManager(resource_manager);
}

BrowserClient::~BrowserClient()
{
	if (win32window && draggable_region)
	{
		OnSetDraggableRegions({});
		DeleteObject(draggable_region);
		draggable_region = 0;
	}
}

void BrowserClient::Create()
{
	CefRefPtr<BrowserClient> client(new BrowserClient());
	CefBrowserSettings browser_settings;
	CefWindowInfo window_info;

#if defined(OS_WIN)
	RECT box;
	client->CreateWin32Window();
	GetClientRect(client->win32window->handle, &box);
	window_info.SetAsChild(client->win32window->handle, box);
#endif

	client->player = std::make_unique<PlayerController>();

	CefBrowserHost::CreateBrowser(window_info, client, "https://webpackage/index.html", browser_settings, nullptr, nullptr);
}

bool BrowserClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	CEF_REQUIRE_UI_THREAD();
	return message_router->OnProcessMessageReceived(browser, frame, source_process, message);
}

void BrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	if (!message_router)
	{
		CefMessageRouterConfig config;
		config.js_query_function = "cefQuery";
		config.js_cancel_function = "cefQueryCancel";
		message_router = CefMessageRouterBrowserSide::Create(config);
		message_handler.reset(new ScriptMessageHandler(this));
		message_router->AddHandler(message_handler.get(), false);
	}

	if (!browser->IsPopup())
	{
		HWND child = browser->GetHost()->GetWindowHandle();
		SetWindowPos(child, 0, 0, 0, win32window->clientWidth(), win32window->clientHeight(), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
	}

	browser_list.push_back(browser);

	if (browser_list.size() == 1)
	{
		//win32window->setPosition(100, 100, 1800, 900);
		win32window->showWindow(false);

// #define SHOW_DEVTOOLS
#ifdef SHOW_DEVTOOLS
		HDC dc = GetDC(win32window->handle);
		int dpi = GetDeviceCaps(dc, LOGPIXELSX);
		ReleaseDC(win32window->handle, dc);
		int devtoolsheight = 300 * dpi / 96;

		RECT box = {};
		GetClientRect(win32window->handle, &box);
		box.top = std::max(box.bottom - devtoolsheight, 0L);
		devtools = CreateWindowEx(0, L"Static", L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, box.left, box.top, box.right - box.left, box.bottom - box.top, win32window->handle, 0, 0, 0);

		CefBrowserSettings settings;
		CefWindowInfo windowInfo;
		RECT box2 = {};
		GetClientRect(devtools, &box2);
		windowInfo.SetAsChild(devtools, box2);
		browser->GetHost()->ShowDevTools(windowInfo, browser->GetHost()->GetClient(), settings, CefPoint(0, 0));

		if (!browser->IsPopup())
		{
			HWND child = browser->GetHost()->GetWindowHandle();
			SetWindowPos(child, 0, 0, 0, win32window->clientWidth(), box.top, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
		}
#endif
	}
}

bool BrowserClient::DoClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose() documentation in the CEF header for a detailed destription of this process.
	if (browser_list.size() == 1)
	{
		is_closing = true;
	}

	return false; // Allow the close. For windowed browsers this will result in the OS close event being sent
}

void BrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	// Remove from the list of existing browsers.
	for (auto bit = browser_list.begin(); bit != browser_list.end(); ++bit)
	{
		if ((*bit)->IsSame(browser))
		{
			browser_list.erase(bit);
			break;
		}
	}

	if (browser_list.empty())
	{
		message_router->RemoveHandler(message_handler.get());
		message_handler.reset();
		message_router = nullptr;

		// All browser windows have closed. Quit the application message loop.
		CefQuitMessageLoop();
	}
}

bool BrowserClient::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool user_gesture, bool is_redirect)
{
	CEF_REQUIRE_UI_THREAD();
	message_router->OnBeforeBrowse(browser, frame);
	return false;
}

void BrowserClient::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message using a data: URI.
	std::string html = "<html><body bgcolor='white'><h2>Failed to load URL " + std::string(failedUrl) + " with error " + std::string(errorText) + " (" + std::to_string(errorCode) + ").</h2></body></html>";
	frame->LoadURL(GetDataURI(html, "text/html"));
}

void BrowserClient::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	CEF_REQUIRE_UI_THREAD();

	CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
	if (hwnd)
		SetWindowText(hwnd, std::wstring(title).c_str());
}

CefRefPtr<CefResourceRequestHandler> BrowserClient::GetResourceRequestHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_navigation, bool is_download, const CefString& request_initiator, bool& disable_default_handling)
{
	CEF_REQUIRE_IO_THREAD();
	return this;
}

void BrowserClient::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
{
	CEF_REQUIRE_UI_THREAD();
	message_router->OnRenderProcessTerminated(browser);
}

cef_return_value_t BrowserClient::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback)
{
	CEF_REQUIRE_IO_THREAD();
	return resource_manager->OnBeforeResourceLoad(browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler> BrowserClient::GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request)
{
	CEF_REQUIRE_IO_THREAD();
	return resource_manager->GetResourceHandler(browser, frame, request);
}

bool BrowserClient::OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask)
{
	CEF_REQUIRE_UI_THREAD();
	if ((mask & DRAG_OPERATION_LINK) && !dragData->IsFragment())
		return true; // Forbid dragging of URLs and files
	return false;
}

void BrowserClient::OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const std::vector<CefDraggableRegion>& regions)
{
	CEF_REQUIRE_UI_THREAD();
	OnSetDraggableRegions(regions);
}

void BrowserClient::CloseAllBrowsers(bool force_close)
{
	if (!CefCurrentlyOn(TID_UI))
	{
		CefPostTask(TID_UI, base::Bind(&BrowserClient::CloseAllBrowsers, this, force_close));
	}
	else
	{
		for (auto it = browser_list.begin(); it != browser_list.end(); ++it)
			(*it)->GetHost()->CloseBrowser(force_close);
	}
}

void BrowserClient::CreateWin32Window()
{
	win32window = std::make_unique<Win32Window>(true, false);
	win32window->eventGeometryChanged = [this]()
	{
#ifdef SHOW_DEVTOOLS
		HDC dc = GetDC(win32window->handle);
		int dpi = GetDeviceCaps(dc, LOGPIXELSX);
		ReleaseDC(win32window->handle, dc);
		int devtoolsheight = 300 * dpi / 96;
#else
		int devtoolsheight = 0;
#endif

		int width = win32window->clientWidth();
		int height = std::max(win32window->clientHeight() - devtoolsheight, 0);
		bool zoomed = IsZoomed(win32window->handle) == TRUE;
		for (auto& browser : browser_list)
		{
			if (!browser->IsPopup())
			{
				HWND child = browser->GetHost()->GetWindowHandle();
				if (zoomed)
					SetWindowPos(child, 0, 8, 8, width - 16, height - 16, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
				else
					SetWindowPos(child, 0, 0, 0, width, height, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
			}
		}

		if (devtools)
		{
			int devheight = win32window->clientHeight() - height;
			if (zoomed)
				SetWindowPos(devtools, 0, 8, 8 + height, width - 16, devheight - 16, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
			else
				SetWindowPos(devtools, 0, 0, height, width, devheight, SWP_NOACTIVATE | SWP_NOOWNERZORDER);

			RECT box;
			GetClientRect(devtools, &box);

			auto callback = [](HWND hwnd, LPARAM lparam) -> BOOL
			{
				const RECT& box = *reinterpret_cast<RECT*>(lparam);
				SetWindowPos(hwnd, 0, 0, 0, box.right, box.bottom, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
				return TRUE;
			};
			EnumChildWindows(devtools, callback, reinterpret_cast<LPARAM>(&box));
		}
	};
	win32window->eventFrameHitTest = [this](int x, int y)
	{
		if (draggable_region && PtInRegion(draggable_region, x, y))
		{
			return HTCAPTION;
		}
		else
		{
			return HTCLIENT;
		}
	};
}

void BrowserClient::OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions)
{
	if (!draggable_region)
		draggable_region = CreateRectRgn(0, 0, 0, 0);

	SetRectRgn(draggable_region, 0, 0, 0, 0);

	for (const CefDraggableRegion& area : regions)
	{
		HRGN region = CreateRectRgn(area.bounds.x, area.bounds.y, area.bounds.x + area.bounds.width, area.bounds.y + area.bounds.height);
		CombineRgn(draggable_region, draggable_region, region, area.draggable ? RGN_OR : RGN_DIFF);
		DeleteObject(region);
	}

	if (win32window)
	{
		if (!regions.empty())
		{
			auto callback = [](HWND hwnd, LPARAM lparam) -> BOOL { reinterpret_cast<BrowserClient*>(lparam)->SubclassWindow(hwnd); return TRUE; };
			EnumChildWindows(win32window->handle, callback, reinterpret_cast<LPARAM>(this));
		}
		else
		{
			auto callback = [](HWND hwnd, LPARAM lparam) -> BOOL { reinterpret_cast<BrowserClient*>(lparam)->UnsubclassWindow(hwnd); return TRUE; };
			EnumChildWindows(win32window->handle, callback, reinterpret_cast<LPARAM>(this));
		}
	}
}

bool BrowserClient::IsDevToolsChild(HWND hwnd) const
{
	if (devtools == 0)
		return false;

	while (hwnd)
	{
		if (hwnd == devtools)
			return true;
		hwnd = GetAncestor(hwnd, GA_PARENT);
	}
	return false;
}

void BrowserClient::SubclassWindow(HWND hwnd)
{
	if (IsDevToolsChild(hwnd))
		return;

	HANDLE parentwndproc = GetPropW(hwnd, L"CefParentWndProc");
	if (parentwndproc)
		return;

	SetLastError(ERROR_SUCCESS);
	LONG_PTR oldwndproc = SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&BrowserClient::SubclassedWindowProc));
	if (oldwndproc == 0 && GetLastError() != ERROR_SUCCESS)
		return;

	SetPropW(hwnd, L"CefParentWndProc", reinterpret_cast<HANDLE>(oldwndproc));
	SetPropW(hwnd, L"CefDraggableRegion", reinterpret_cast<HANDLE>(draggable_region));
}

void BrowserClient::UnsubclassWindow(HWND hwnd)
{
	if (IsDevToolsChild(hwnd))
		return;

	HANDLE parentwndproc = GetPropW(hwnd, L"CefParentWndProc");
	if (parentwndproc)
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(parentwndproc));

	RemovePropW(hwnd, L"CefParentWndProc");
	RemovePropW(hwnd, L"CefDraggableRegion");
}

LRESULT BrowserClient::SubclassedWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	WNDPROC parentwndproc = reinterpret_cast<WNDPROC>(GetPropW(hwnd, L"CefParentWndProc"));
	HRGN region = reinterpret_cast<HRGN>(GetPropW(hwnd, L"CefDraggableRegion"));

	if (message == WM_NCHITTEST)
	{
		LRESULT hit = CallWindowProc(parentwndproc, hwnd, message, wparam, lparam);
		if (hit == HTCLIENT)
		{
			POINTS points = MAKEPOINTS(lparam);
			POINT point = { points.x, points.y };
			ScreenToClient(hwnd, &point);
			if (PtInRegion(region, point.x, point.y))
			{
				return HTTRANSPARENT; // Let the parent window handle WM_NCHITTEST
			}
		}
	}

	return CallWindowProc(parentwndproc, hwnd, message, wparam, lparam);
}

void BrowserClient::SetupResourceManager(CefRefPtr<CefResourceManager> resource_manager)
{
	/*if (!CefCurrentlyOn(TID_IO))
	{
		CefPostTask(TID_IO, base::Bind(SetupResourceManager, resource_manager));
	}
	else*/
	{
		resource_manager->AddArchiveProvider("https://webpackage/", FilePath::combine(Directory::exe_path(), "MusicPlayer.webpkg"), std::string(), 0, std::string());
	}
}

std::string BrowserClient::GetDataURI(const std::string& data, const std::string& mime_type)
{
	return "data:" + mime_type + ";base64," + CefURIEncode(CefBase64Encode(data.data(), data.size()), false).ToString();
}

ScriptMessageHandler::ScriptMessageHandler(BrowserClient* client) : client(client)
{
	thread = std::thread([this]() { WorkerThreadMain(); });
}

ScriptMessageHandler::~ScriptMessageHandler()
{
	std::unique_lock<std::mutex> lock(mutex);
	stopflag = true;
	lock.unlock();
	condvar.notify_all();
	thread.join();
}

void ScriptMessageHandler::WorkerThreadMain()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(mutex);
		condvar.wait(lock, [&]() { return stopflag || !requests.empty(); });
		if (stopflag)
			break;

		std::vector<QueuedRequest> queue;
		queue.swap(requests);
		lock.unlock();

		for (const QueuedRequest& q : queue)
		{
			try
			{
				const JsonValue& requestJson = q.request;
				JsonValue responseJson = JsonValue::object();

				std::string command = requestJson["command"].to_string();
				if (command == "play")
				{
					std::string filename = requestJson["filename"].to_string();
					client->player->play(filename);
				}
				else if (command == "stop")
				{
					client->player->stop();
				}
				else if (command == "searchfolder")
				{
					std::string path = requestJson["path"].to_string();

					SearchFolderResult searchresult = client->player->searchfolder(path);

					JsonValue files = JsonValue::array();
					for (const std::string& file : searchresult.files)
					{
						files.items().push_back(JsonValue::string(file));
					}
					responseJson["files"] = files;

					JsonValue folders = JsonValue::array();
					for (const std::string& folder : searchresult.folders)
					{
						folders.items().push_back(JsonValue::string(folder));
					}
					responseJson["folders"] = folders;
				}
				else if (command == "playlist")
				{
					JsonValue items = JsonValue::array();
					for (const PlaylistItem& listitem : client->player->get_playlist())
					{
						JsonValue item = JsonValue::object();
						item["filename"].set_string(listitem.filename);
						item["name"].set_string(listitem.name);
						items.items().push_back(item);
					}
					responseJson["items"] = items;
				}
				else if (command == "currentplayback")
				{
					CurrentPlaybackInfo info = client->player->get_current_playback();
					responseJson["name"].set_string(info.item.name);
					responseJson["filename"].set_string(info.item.filename);
					responseJson["secondsPosition"].set_number(info.seconds_position);
					responseJson["secondsLength"].set_number(info.seconds_length);
				}
				else
				{
					throw std::runtime_error("Unknown or missing command in request");
				}

				q.callback->Success(responseJson.to_json());
			}
			catch (const std::exception& e)
			{
				q.callback->Failure(-1, e.what());
			}
		}
	}
}

bool ScriptMessageHandler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback)
{
	try
	{
		JsonValue requestJson = JsonValue::parse(request);
		JsonValue responseJson = JsonValue::object();

		std::string command = requestJson["command"].to_string();
		if (command == "minimize")
		{
			PostMessage(client->win32window->handle, WM_SYSCOMMAND, SC_MINIMIZE, 0);
			callback->Success(responseJson.to_json());
		}
		else if (command == "maximize")
		{
			if (IsZoomed(client->win32window->handle))
				PostMessage(client->win32window->handle, WM_SYSCOMMAND, SC_RESTORE, 0);
			else
				PostMessage(client->win32window->handle, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			callback->Success(responseJson.to_json());
		}
		else if (command == "close")
		{
			PostMessage(client->win32window->handle, WM_SYSCOMMAND, SC_CLOSE, 0);
			callback->Success(responseJson.to_json());
		}
		else
		{
			std::unique_lock<std::mutex> lock(mutex);
			requests.emplace_back(std::move(callback), std::move(requestJson));
			lock.unlock();
			condvar.notify_all();
		}
	}
	catch (const std::exception& e)
	{
		callback->Failure(-1, e.what());
	}

	return true;
}
