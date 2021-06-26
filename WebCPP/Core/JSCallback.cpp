
#include "JSCallback.h"
#include <emscripten/emscripten.h>

extern "C"
{
	void EMSCRIPTEN_KEEPALIVE invoke_js_callback(JSCallback* callback)
	{
		callback->invoke();
	}
}

JSCallback::JSCallback(std::function<JSValue(JSValue args)> callback, bool deleteOnInvoke) : callback(std::move(callback)), handle(JSValue::global("JSCallback").new_((ptrdiff_t)this)), deleteOnInvoke(deleteOnInvoke)
{
}

JSCallback::~JSCallback()
{
	handle.call<JSValue>("release");
}

JSValue JSCallback::getHandler()
{
	return handle.call<JSValue>("getHandler");
}

void JSCallback::invoke()
{
	// Make a copy since ~JSCallback may be called by the callback
	JSValue h = handle;
	std::function<JSValue(JSValue args)> c = callback;
	if (deleteOnInvoke)
		delete this;
	h.call<void>("setResult", c(h.call<JSValue>("getArgs")));
}
