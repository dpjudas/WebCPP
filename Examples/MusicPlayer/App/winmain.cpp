
#include <windows.h>

#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"
#include "browser_app.h"

#if defined(CEF_USE_SANDBOX)
#pragma comment(lib, "cef_sandbox.lib")
#endif

// Entry point function for all processes.
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	CefEnableHighDPISupport();

	#if defined(CEF_USE_SANDBOX)
	CefScopedSandboxInfo scoped_sandbox;
	void* sandbox_info = scoped_sandbox.sandbox_info();
	#else
	void* sandbox_info = nullptr;
	#endif

	CefMainArgs main_args(hInstance);
	CefRefPtr<BrowserApp> app(new BrowserApp);

	int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
	if (exit_code >= 0)
		return exit_code;

	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());

	CefSettings settings;

	//CefString(&settings.cache_path).FromString("C:\\Development\\musicplayercache");
	//settings.persist_user_preferences = true;

	#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
	#endif

	CefInitialize(main_args, settings, app.get(), sandbox_info);

	CefRunMessageLoop();
	CefShutdown();

	return 0;
}
