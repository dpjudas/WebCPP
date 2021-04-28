
#include "SvgBox.h"

SvgBox::SvgBox(View* parent) : View(parent, xmlns, "svg")
{
	addClass("svgbox");
}

void SvgBox::setViewbox(double x, double y, double width, double height)
{
	element->setAttribute("viewBox", std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(width) + " " + std::to_string(height));
}

void SvgBox::clear()
{
	while (true)
	{
		auto child = element->handle["lastChild"];
		if (child.isNull()) break;
		element->handle.call<void>("removeChild", child);
	}
}

void SvgBox::addElement(std::string tag, std::map<std::string, std::string> attributes)
{
	element->handle.call<void>("appendChild", createElement(tag, attributes));
}

emscripten::val SvgBox::createElement(std::string tag, std::map<std::string, std::string> attributes)
{
	auto svgelement = JSValue::global("document").call<JSValue>("createElementNS", xmlns, tag);
	for (auto& it : attributes)
	{
		svgelement.call<void>("setAttribute", it.first, it.second);
	}
	return svgelement;
}

const std::string SvgBox::xmlns = "http://www.w3.org/2000/svg";
