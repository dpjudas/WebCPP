
#include "JSCallback.h"
#include <emscripten/emscripten.h>

JSCallback::JSCallback(std::function<JSValue(JSValue args)> callback, bool deleteOnInvoke) : callback(std::move(callback)), handle(JSValue::global("Module")["JSCallback"].new_((ptrdiff_t)this)), deleteOnInvoke(deleteOnInvoke)
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

extern "C"
{
    EMSCRIPTEN_KEEPALIVE
	void invoke_js_callback(JSCallback* callback)
	{
		callback->invoke();
	}
}

static struct ModuleJSCallback
{
    ModuleJSCallback()
	{
		EM_ASM(

            Module.JSCallback = function(id) {
                this.id = id;
                this.args = [];
                this.retval = undefined;

                this.getHandler = function() {
                    return this.handler.bind(this);
                };

                this.getArgs = function() {
                    return this.args;
                };

                this.setResult = function(value) {
                    this.retval = value;
                };

                this.release = function() {
                    this.id = null;
                };

                this.handler = function() {
                    if (this.id != null) {
                        this.args = Array.from(arguments);
                        _invoke_js_callback(this.id);
                        var result = this.retval;
                        this.args = [];
                        this.retval = undefined;
                        return result;
                    }
                };
            }

		);
	}
} modulejs;
