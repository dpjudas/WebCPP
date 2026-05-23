
#include "WebCPP/Core/HtmlDocument.h"
#include "WebCPP/Core/JSValue.h"

namespace web
{
	void HtmlDocument::setTitle(const std::string& title)
	{
		JSValue::global("document").set("title", title);
	}

	std::string HtmlDocument::getTitle()
	{
		return JSValue::global("document")["title"].as<std::string>();
	}

	void HtmlDocument::setIcon(const std::string& href)
	{
		JSValue element = JSValue::global("document")["head"]["firstElementChild"];
		while (!element.isNull())
		{
			if (element["nodeName"].as<std::string>() == "LINK" && element["rel"].as<std::string>() == "icon")
			{
				element.set("href", href);
				return;
			}
			element = element["nextElementSibling"];
		}

		element = JSValue::global("document").call<JSValue>("createElement", std::string("link"));
		element.set("rel", "icon");
		element.set("href", href);
		JSValue::global("document")["head"].call<void>("appendChild", element);
	}

	std::string HtmlDocument::getIcon()
	{
		JSValue element = JSValue::global("document")["head"]["firstElementChild"];
		while (!element.isNull())
		{
			if (element["nodeName"].as<std::string>() == "LINK" && element["rel"].as<std::string>() == "icon")
			{
				return element["href"].as<std::string>();
			}
			element = element["nextElementSibling"];
		}
		return {};
	}

	HtmlDocumentBody* HtmlDocument::body()
	{
		static std::shared_ptr<HtmlDocumentBody> instance = std::make_shared<HtmlDocumentBody>();
		return instance.get();
	}

	////////////////////////////////////////////////////////////////////////////

	HtmlDocumentBody::HtmlDocumentBody() : View(std::make_unique<Element>(JSValue::global("document")["body"]))
	{
		createFlowLayout();
	}

	void HtmlDocumentBody::addView(std::shared_ptr<View> view)
	{
		getLayout<FlowLayout>()->addView(view);
	}

	std::shared_ptr<ModalLayer> HtmlDocumentBody::beginDialogModal()
	{
		JSValue activeElement = JSValue::global("document")["activeElement"];
		if (!modalLayers.empty())
			modalLayers.back()->element->setAttribute("inert", "");
		auto layer = std::make_shared<ModalLayer>(true);
		layer->oldActiveElement = std::move(activeElement);
		getLayout<FlowLayout>()->addView(layer);
		modalLayers.push_back(layer);
		return layer;
	}

	std::shared_ptr<ModalLayer> HtmlDocumentBody::beginPopupModal()
	{
		JSValue activeElement = JSValue::global("document")["activeElement"];
		if (!modalLayers.empty())
			modalLayers.back()->element->setAttribute("inert", "");
		auto layer = std::make_shared<ModalLayer>(false);
		layer->oldActiveElement = std::move(activeElement);
		getLayout<FlowLayout>()->addView(layer);
		modalLayers.push_back(layer);
		return layer;
	}

	void HtmlDocumentBody::endModal()
	{
		if (!modalLayers.empty())
		{
			auto layer = modalLayers.back();
			modalLayers.pop_back();
			layer->detach();
			JSValue oldActiveElement = std::move(layer->oldActiveElement);
			if (!modalLayers.empty())
				modalLayers.back()->element->removeAttribute("inert");
			if (!oldActiveElement.isNull())
				oldActiveElement.call<void>("focus");
			if (!modalLayers.empty())
			{
				JSValue active = JSValue::global("document")["activeElement"];
				if (active == element->handle)
					modalLayers.back()->focusFirstChild();
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////

	ModalLayer::ModalLayer(bool dialog) : View("modallayer-view")
	{
		if (dialog)
			addClass("shaded");

		oldActiveElement = JSValue::global("document")["activeElement"];
		createFlowLayout();
	}

	void ModalLayer::addView(std::shared_ptr<View> view)
	{
		getLayout<FlowLayout>()->addView(view);
	}
}
