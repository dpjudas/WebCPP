
#include "WebCPP/Controls/SvgBox/SvgBox.h"

namespace web
{
	void SvgElementView::clear()
	{
		// Remove attached views
		if (getLayout())
			getLayout()->clear();

		// Remove elements created without a view
		while (true)
		{
			auto child = element->handle["lastChild"];
			if (child.isNull()) break;
			element->handle.call<void>("removeChild", child);
		}
	}

	void SvgElementView::addElement(std::string tag, std::map<std::string, std::string> attributes)
	{
		element->handle.call<void>("appendChild", createElement(tag, attributes));
	}

	void SvgElementView::addElement(std::string tag, std::string text, std::map<std::string, std::string> attributes)
	{
		element->handle.call<void>("appendChild", createElement(tag, text, attributes));
	}

	JSValue SvgElementView::createElement(std::string tag, std::map<std::string, std::string> attributes)
	{
		auto svgelement = JSValue::global("document").call<JSValue>("createElementNS", xmlns, tag);
		for (auto& it : attributes)
		{
			svgelement.call<void>("setAttribute", it.first, it.second);
		}
		return svgelement;
	}

	JSValue SvgElementView::createElement(std::string tag, std::string text, std::map<std::string, std::string> attributes)
	{
		auto document = JSValue::global("document");
		auto svgelement = document.call<JSValue>("createElementNS", xmlns, tag);
		for (auto& it : attributes)
		{
			svgelement.call<void>("setAttribute", it.first, it.second);
		}
		svgelement.call<void>("appendChild", document.call<JSValue>("createTextNode", text));
		return svgelement;
	}

	const std::string SvgElementView::xmlns = "http://www.w3.org/2000/svg";

	/////////////////////////////////////////////////////////////////////////////

	SvgBox::SvgBox() : SvgElementView("svg")
	{
		addClass("svgbox");
		createFlowLayout();
	}

	void SvgBox::setViewbox(double x, double y, double width, double height)
	{
		element->setAttribute("viewBox", std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(width) + " " + std::to_string(height));
	}

	void SvgBox::addView(std::shared_ptr<SvgElementView> view)
	{
		getLayout<FlowLayout>()->addView(view);
	}
}
