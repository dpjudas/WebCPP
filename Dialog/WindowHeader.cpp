
#include "WindowHeader.h"
#include "DialogHeader.h"
#include "DialogHeaderButton.h"
#include "Dialog.h"
#include "WebCPP/Core/Native.h"

WindowHeader::WindowHeader(View* p) : HBoxView(p)
{
	caption = new TextLabel(this);
	minimizeButton = new DialogHeaderButton(this, DialogHeader::MinimizeIconSvg);
	maximizeButton = new DialogHeaderButton(this, DialogHeader::MaximizeIconSvg);
	closeButton = new DialogHeaderButton(this, DialogHeader::CloseIconSvg);

	addClass("windowheader");
	caption->addClass("windowheader-caption");
	minimizeButton->addClass("windowheader-minbutton");
	maximizeButton->addClass("windowheader-maxbutton");
	closeButton->addClass("windowheader-closebutton");

	caption->setExpanding();

	minimizeButton->element->addEventListener("click", [this](Event* event) { event->stopPropagation(); onMinimize(); });
	maximizeButton->element->addEventListener("click", [this](Event* event) { event->stopPropagation(); onMaximize(); });
	closeButton->element->addEventListener("click", [this](Event* event) { event->stopPropagation(); onClose(); });
}

void WindowHeader::onMinimize()
{
	JSValue request = JSValue::object();
	request.set("command", std::string("minimize"));
	Native::get()->query(request);
}

void WindowHeader::onMaximize()
{
	JSValue request = JSValue::object();
	request.set("command", std::string("maximize"));
	Native::get()->query(request);
}

void WindowHeader::onClose()
{
	JSValue request = JSValue::object();
	request.set("command", std::string("close"));
	Native::get()->query(request);
}
