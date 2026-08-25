
#include "WebCPP/Core/ScriptLoader.h"
#include "WebCPP/Core/Element.h"
#include "WebCPP/Core/Event.h"
#include "WebCPP/Core/JSCallback.h"

#include <list>
#include <map>

namespace web
{
	namespace
	{
		enum class LoadState { Loading, Loaded, Failed };

		struct ScriptEntry
		{
			LoadState state = LoadState::Loading;
			std::unique_ptr<Element> element;
			std::vector<std::shared_ptr<task_promise<>>> waiters;
			std::string errorMessage;
			ScriptLoadOptions options;
		};

		struct AmdEntry
		{
			LoadState state = LoadState::Loading;
			std::vector<std::shared_ptr<task_promise<>>> waiters;
			std::string errorMessage;
			std::unique_ptr<JSCallback> successCallback;
			std::unique_ptr<JSCallback> errorCallback;
		};

		std::map<std::string, std::shared_ptr<ScriptEntry>>& scriptEntries()
		{
			static std::map<std::string, std::shared_ptr<ScriptEntry>> entries;
			return entries;
		}

		std::map<std::string, std::shared_ptr<AmdEntry>>& amdEntries()
		{
			static std::map<std::string, std::shared_ptr<AmdEntry>> entries;
			return entries;
		}

		int& amdSuppressDepth()
		{
			static int depth = 0;
			return depth;
		}

		JSValue& savedAmdDefine()
		{
			static JSValue value = JSValue::undefined();
			return value;
		}

		void pushAmdSuppress()
		{
			if (amdSuppressDepth()++ == 0)
			{
				JSValue window = JSValue::global("window");
				savedAmdDefine() = window["define"];
				window.set("define", JSValue::undefined());
			}
		}

		void popAmdSuppress()
		{
			if (--amdSuppressDepth() == 0)
			{
				JSValue::global("window").set("define", savedAmdDefine());
				savedAmdDefine() = JSValue::undefined();
			}
		}

		bool isGlobalDefined(const std::string& name)
		{
			if (name.empty())
				return true;
			JSValue value = JSValue::global(name.c_str());
			return !value.isUndefined() && !value.isNull();
		}

		void resolveWaiters(std::vector<std::shared_ptr<task_promise<>>>& waiters)
		{
			std::vector<std::shared_ptr<task_promise<>>> ready = std::move(waiters);
			waiters.clear();
			for (auto& waiter : ready)
				waiter->set_value();
		}

		void rejectWaiters(std::vector<std::shared_ptr<task_promise<>>>& waiters, const std::string& message)
		{
			std::vector<std::shared_ptr<task_promise<>>> ready = std::move(waiters);
			waiters.clear();
			std::exception_ptr exception = std::make_exception_ptr(std::runtime_error(message));
			for (auto& waiter : ready)
				waiter->set_exception(exception);
		}

		void completeScriptSuccess(const std::shared_ptr<ScriptEntry>& entry)
		{
			if (entry->options.suppressAmdDefine)
				popAmdSuppress();

			if (!isGlobalDefined(entry->options.globalName))
			{
				entry->state = LoadState::Failed;
				entry->errorMessage = "Script loaded but global '" + entry->options.globalName + "' was not defined";
				rejectWaiters(entry->waiters, entry->errorMessage);
				return;
			}

			entry->state = LoadState::Loaded;
			resolveWaiters(entry->waiters);
		}

		void completeScriptFailure(const std::shared_ptr<ScriptEntry>& entry, const std::string& message)
		{
			if (entry->options.suppressAmdDefine)
				popAmdSuppress();

			entry->state = LoadState::Failed;
			entry->errorMessage = message;
			rejectWaiters(entry->waiters, message);
		}

		void startScriptLoad(const std::string& url, const std::shared_ptr<ScriptEntry>& entry)
		{
			entry->state = LoadState::Loading;
			entry->errorMessage.clear();

			if (entry->options.suppressAmdDefine)
				pushAmdSuppress();

			entry->element = std::make_unique<Element>("script");
			entry->element->setAttribute("src", url);

			std::weak_ptr<ScriptEntry> weak = entry;
			entry->element->addEventListener("load", [weak](Event*)
			{
				if (auto self = weak.lock())
					completeScriptSuccess(self);
			});
			entry->element->addEventListener("error", [weak, url](Event*)
			{
				if (auto self = weak.lock())
					completeScriptFailure(self, "Failed to load script: " + url);
			});

			JSValue::global("document")["head"].call<void>("appendChild", entry->element->handle);
		}

		task<> attachCallbacks(task<> loadTask, std::function<void()> onReady, std::function<void(std::string)> onError)
		{
			try
			{
				co_await std::move(loadTask);
				if (onReady)
					onReady();
			}
			catch (const std::exception& ex)
			{
				if (onError)
					onError(ex.what());
			}
		}

		std::list<task<>>& pendingCallbackTasks()
		{
			static std::list<task<>> tasks;
			return tasks;
		}

		void runDetached(task<> callbackTask)
		{
			std::list<task<>>& tasks = pendingCallbackTasks();
			for (auto it = tasks.begin(); it != tasks.end(); )
			{
				if (it->ready())
					it = tasks.erase(it);
				else
					++it;
			}
			tasks.push_back(std::move(callbackTask));
		}
	}

	task<> ensureScript(std::string url, ScriptLoadOptions options)
	{
		auto promise = std::make_shared<task_promise<>>();
		auto& entries = scriptEntries();
		auto it = entries.find(url);

		if (it != entries.end())
		{
			std::shared_ptr<ScriptEntry> entry = it->second;
			if (entry->state == LoadState::Loaded)
			{
				if (!isGlobalDefined(entry->options.globalName))
				{
					promise->set_exception(std::make_exception_ptr(std::runtime_error(
						"Script previously loaded but global '" + entry->options.globalName + "' is not defined")));
				}
				else
				{
					promise->set_value();
				}
				return promise->get_future();
			}

			if (entry->state == LoadState::Loading)
			{
				entry->waiters.push_back(promise);
				return promise->get_future();
			}

			// Failed — retry with the new caller's options
			entry->options = std::move(options);
			entry->element.reset();
			entry->waiters.push_back(promise);
			startScriptLoad(url, entry);
			return promise->get_future();
		}

		auto entry = std::make_shared<ScriptEntry>();
		entry->options = std::move(options);
		entry->waiters.push_back(promise);
		entries[url] = entry;
		startScriptLoad(url, entry);
		return promise->get_future();
	}

	void ensureScript(std::string url, ScriptLoadOptions options, std::function<void()> onReady, std::function<void(std::string error)> onError)
	{
		runDetached(attachCallbacks(ensureScript(std::move(url), std::move(options)), std::move(onReady), std::move(onError)));
	}

	task<> ensureAmdModule(std::string moduleId)
	{
		auto promise = std::make_shared<task_promise<>>();

		JSValue require = JSValue::global("require");
		if (require.isUndefined() || require.isNull())
		{
			promise->set_exception(std::make_exception_ptr(std::runtime_error(
				"window.require is not available — load an AMD loader (e.g. RequireJS) before ensureAmdModule")));
			return promise->get_future();
		}

		// Dedupe in C++. Do not call require.defined — Monaco's vs/loader.js does not expose it (unlike full RequireJS); invoking it throws "is not a function". require([id], cb) still fires cb immediately when the module is already loaded
		auto& entries = amdEntries();
		auto it = entries.find(moduleId);
		if (it != entries.end())
		{
			std::shared_ptr<AmdEntry> entry = it->second;
			if (entry->state == LoadState::Loaded)
			{
				promise->set_value();
				return promise->get_future();
			}
			if (entry->state == LoadState::Loading)
			{
				entry->waiters.push_back(promise);
				return promise->get_future();
			}

			// Failed — retry below by replacing the entry
			entries.erase(it);
		}

		auto entry = std::make_shared<AmdEntry>();
		entry->waiters.push_back(promise);
		entries[moduleId] = entry;

		std::weak_ptr<AmdEntry> weak = entry;
		entry->successCallback = std::make_unique<JSCallback>([weak](JSValue) -> JSValue
		{
			if (auto self = weak.lock())
			{
				self->state = LoadState::Loaded;
				resolveWaiters(self->waiters);
			}
			return JSValue::undefined();
		});
		entry->errorCallback = std::make_unique<JSCallback>([weak, moduleId](JSValue args) -> JSValue
		{
			if (auto self = weak.lock())
			{
				std::string message = "Failed to load AMD module: " + moduleId;
				if (args["length"].as<int>() > 0 && !args[0].isUndefined() && !args[0].isNull())
				{
					JSValue err = args[0];
					if (!err["message"].isUndefined())
						message = err["message"].as<std::string>();
					else if (err.isString())
						message = err.as<std::string>();
				}
				self->state = LoadState::Failed;
				self->errorMessage = message;
				rejectWaiters(self->waiters, message);
			}
			return JSValue::undefined();
		});

		std::vector<std::string> deps = { moduleId };
		require(JSValue::array(deps), entry->successCallback->getHandler(), entry->errorCallback->getHandler());
		return promise->get_future();
	}

	void ensureAmdModule(std::string moduleId, std::function<void()> onReady, std::function<void(std::string error)> onError)
	{
		runDetached(attachCallbacks(ensureAmdModule(std::move(moduleId)), std::move(onReady), std::move(onError)));
	}
}
