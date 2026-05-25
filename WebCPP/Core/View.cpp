
#include "WebCPP/Core/View.h"
#include "WebCPP/Core/HtmlDocument.h"
#include "WebCPP/Core/ShadowRoot.h"
#include <stdexcept>

namespace web
{
	View::View(std::string elementType)
	{
		element = std::make_unique<Element>(elementType);
	}

	View::View(std::string ns, std::string elementType)
	{
		element = std::make_unique<Element>(ns, elementType);
		element->setAttribute("xmlns", ns);
	}

	View::View(std::unique_ptr<Element> element) : element(std::move(element))
	{
	}

	View::~View()
	{
	}

	void View::detach()
	{
		if (layoutItem)
			layoutItem->layout->detachItem(layoutItem);
	}

	void View::addClass(std::string name)
	{
		if (classes.insert(name).second)
			updateClassAttribute();
	}

	void View::removeClass(std::string name)
	{
		if (classes.erase(name) > 0)
			updateClassAttribute();
	}

	bool View::hasClass(std::string name) const
	{
		return classes.find(name) != classes.cend();
	}

	void View::updateClassAttribute()
	{
		std::string attr;
		for (const std::string& cls : classes)
		{
			if (!attr.empty()) attr += " ";
			attr += cls;
		}
		element->setAttribute("class", attr);
	}

	void View::setHidden(bool value)
	{
		if (value)
			addClass("hidden");
		else
			removeClass("hidden");
	}

	bool View::getHidden() const
	{
		return hasClass("hidden");
	}

	bool View::applyDefaultFocus()
	{
		return layout ? layout->applyDefaultFocus() : false;
	}

	bool View::focusFirstChild()
	{
		return layout ? layout->focusFirstItem() : false;
	}

	std::shared_ptr<ModalLayer> View::showDialogModal(bool setFocus)
	{
		auto layer = std::make_shared<ModalLayer>(true, [this](Event* e) { onModalCancel(e); });
		modalLayers.push_back(layer);
		HtmlDocument::body()->addView(layer);
		layer->addView(shared_from_this());
		layer->showModal();
		onModalAttach();
		if (!applyDefaultFocus() && !focusFirstChild())
		{
			element->setTabIndex(0);
			element->focus();
		}
		return layer;
	}

	std::shared_ptr<ModalLayer> View::showPopupModal(bool setFocus)
	{
		auto layer = std::make_shared<ModalLayer>(false, [this](Event* e) { onModalCancel(e); });
		modalLayers.push_back(layer);
		HtmlDocument::body()->addView(layer);
		layer->addView(shared_from_this());
		layer->showModal();
		onModalAttach();
		if (setFocus && !applyDefaultFocus() && !focusFirstChild())
		{
			element->setTabIndex(0);
			element->focus();
		}
		return layer;
	}

	void View::closeModal()
	{
		detach();

		if (!modalLayers.empty())
		{
			auto layer = modalLayers.back();
			modalLayers.pop_back();
			layer->close();
			layer->detach();

			JSValue oldActiveElement = std::move(layer->oldActiveElement);
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

	void View::setDefaultFocused()
	{
		defaultFocused = true;
	}

	bool View::setFocus()
	{
		element->focus();
		return false;
	}

	void View::attachShadow(const std::string& mode)
	{
		if (!shadowRoot)
		{
			JSValue options = JSValue::object();
			options.set("mode", mode);
			shadowRoot = std::make_unique<ShadowRoot>(element->handle.call<JSValue>("attachShadow", options));
		}
	}

	View* View::parent() const
	{
		return layoutItem ? layoutItem->layout->owner : nullptr;
	}

	std::vector<std::shared_ptr<ModalLayer>> View::modalLayers;

	////////////////////////////////////////////////////////////////////////////

	ModalLayer::ModalLayer(bool dialog, std::function<void(Event* event)> onCancelCallback) : View("dialog"), onCancelCallback(onCancelCallback)
	{
		addClass("modallayer");
		oldActiveElement = JSValue::global("document")["activeElement"];
		element->addEventListener("cancel", std::bind_front(&ModalLayer::onCancel, this));
		if (dialog)
			addClass("shaded");
		createFlowLayout();
	}

	void ModalLayer::addView(std::shared_ptr<View> view)
	{
		getLayout<FlowLayout>()->addView(view);
	}

	void ModalLayer::showModal()
	{
		element->handle.call<void>("showModal");
	}

	void ModalLayer::close()
	{
		element->handle.call<void>("close");
	}

	void ModalLayer::onCancel(Event* event)
	{
		event->preventDefault(); // Prevent native close; we manage lifetime via endModal()
		if (onCancelCallback)
			onCancelCallback(event);
	}
}

/*
static struct JSCode { JSCode() { emscripten_run_script(R"jscode(

	class ViewElement extends HTMLElement { constructor() { super(); } };
	window.customElements.define("basic-view", ViewElement);

)jscode"); } } initJSCode;
*/
