#pragma once

#include "Task.h"

#include <functional>
#include <string>

namespace web
{
	// Options for ensureScript. First caller for a given URL wins; later callers with the same URL share that in-flight/completed load (their options are ignored)
	struct ScriptLoadOptions
	{
		// Temporarily clear window.define while the script loads so UMD builds assign a window global instead of registering as an anonymous AMD module.
		// Needed when a RequireJS-style loader (e.g. Monaco's vs/loader.js) is already on the page.
		bool suppressAmdDefine = false;

		// If non-empty, after load require JSValue::global(globalName) to be defined; otherwise the load fails (guards against silent AMD mis-registration)
		std::string globalName;
	};

	// Load a classic <script src> once per URL. Concurrent callers share one in-flight load.
	// Throws std::runtime_error on network/script error or a missing globalName.
	task<> ensureScript(std::string url, ScriptLoadOptions options = {});

	// Callback form — same dual-API shape as sendRequest.
	void ensureScript(std::string url, ScriptLoadOptions options, std::function<void()> onReady, std::function<void(std::string error)> onError = {});

	// Load an AMD module via window.require([moduleId], ...). Deduped by module id.
	// Does not call require.config — the app configures paths once. Throws if require is absent or the module fails to load.
	task<> ensureAmdModule(std::string moduleId);

	void ensureAmdModule(std::string moduleId, std::function<void()> onReady, std::function<void(std::string error)> onError = {});
}
