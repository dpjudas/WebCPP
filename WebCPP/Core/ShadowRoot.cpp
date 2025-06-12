
#include "WebCPP/Core/ShadowRoot.h"
#include "WebCPP/Core/Element.h"
#include <stdio.h>
#include <emscripten/emscripten.h>

class ComponentStyleSheet
{
public:
	ComponentStyleSheet() : handle(JSValue::global("CSSStyleSheetHelper").call<JSValue>("create"/*, createOptions()*/))
	{
	}

	void replaceSync(const std::string& value)
	{
		handle.call<void>("replaceSync", value);
	}

	JSValue handle;

private:
	/* Only Firefox supports this in Jan 2025. Maybe we can use it later?
	static JSValue createOptions()
	{
		JSValue options = JSValue::object();
		options.set("baseURL", JSValue::module_property("resources"));
		return options;
	}
	*/
};

static ComponentStyleSheet* getComponentStyleSheet()
{
	static ComponentStyleSheet sheet;
	return &sheet;
}

void ShadowRoot::setComponentStyleSheet(const std::string& value)
{
	getComponentStyleSheet()->replaceSync(value);
}

ShadowRoot::ShadowRoot(JSValue handle) : handle(std::move(handle))
{
	ComponentStyleSheet* sheet = getComponentStyleSheet();
	this->handle["adoptedStyleSheets"].call<void>("push", sheet->handle);
}

ShadowRoot::~ShadowRoot()
{
}

void ShadowRoot::insertBefore(Element* newElement, Element* insertPoint)
{
	handle.call<void>("insertBefore", newElement->handle, insertPoint->handle);
}

void ShadowRoot::appendChild(Element* newElement)
{
	handle.call<void>("appendChild", newElement->handle);
}

void ShadowRoot::removeChild(Element* childElement)
{
	handle.call<void>("removeChild", childElement->handle);
}

// There is something wrong with emscripten::val::new_ that makes it not work :(
static struct JSCode { JSCode() { emscripten_run_script(R"jscode(

	CSSStyleSheetHelper = {
		create: function(options) {
			return new CSSStyleSheet(options);
		}
	};

)jscode");
	}
} initJSCode;
