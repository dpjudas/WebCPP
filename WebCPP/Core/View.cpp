
#include "WebCPP/Core/View.h"
#include "WebCPP/Core/DocumentBodyView.h"
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
		while (lastChildObj)
			delete lastChildObj;

		detachFromParent(true);
		layout.reset();
	}

	void View::setParent(View* newParent)
	{
		if (parentObj != newParent)
		{
			if (parentObj)
				detachFromParent(true);

			if (newParent)
			{
				prevSiblingObj = newParent->lastChildObj;
				if (prevSiblingObj) prevSiblingObj->nextSiblingObj = this;
				newParent->lastChildObj = this;
				if (!newParent->firstChildObj) newParent->firstChildObj = this;
				parentObj = newParent;
			}
		}
	}

	void View::detachFromParent(bool notifyLayout)
	{
		if (parentObj)
		{
			if (notifyLayout && parentObj->layout)
				parentObj->layout->onViewRemoved(this);
		}

		if (prevSiblingObj)
			prevSiblingObj->nextSiblingObj = nextSiblingObj;
		if (nextSiblingObj)
			nextSiblingObj->prevSiblingObj = prevSiblingObj;
		if (parentObj)
		{
			if (parentObj->firstChildObj == this)
				parentObj->firstChildObj = nextSiblingObj;
			if (parentObj->lastChildObj == this)
				parentObj->lastChildObj = prevSiblingObj;
		}
		prevSiblingObj = nullptr;
		nextSiblingObj = nullptr;
		parentObj = nullptr;
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

	bool View::forceFocus()
	{
		for (View* cur = firstChild(); cur != nullptr; cur = cur->nextSibling())
		{
			if (cur->getHidden() == false)
			{
				if (cur->defaultFocused == true)
				{
					cur->setFocus();
					return true;
				}
				else if (cur->forceFocus() == true)
				{
					return true;
				}
			}
		}
		return false;
	}

	bool View::focusFirstChild()
	{
		for (View* cur = firstChild(); cur != nullptr; cur = cur->nextSibling())
		{
			if (cur->getHidden() == false)
			{
				if (cur->setFocus() == true)
				{
					cur->setFocus();
					return true;
				}
				else if (cur->focusFirstChild() == true)
				{
					return true;
				}
			}
		}
		return false;
	}

	ModalLayer* View::showModal()
	{
		auto layer = DocumentBodyView::get()->beginModal();
		setParent(layer);
		auto layout = layer->createFlowLayout();
		layout->addView(this);
		onModalAttach();
		if (forceFocus() == false && focusFirstChild() == false)
		{
			element->setTabIndex(0);
			element->focus();
		}
		return layer;
	}

	ModalLayer* View::showUnshadedModal(const bool setFocus)
	{
		auto layer = DocumentBodyView::get()->beginUnshadedModal();
		setParent(layer);
		auto layout = layer->createFlowLayout();
		layout->addView(this);
		onModalAttach();
		if (setFocus == true && forceFocus() == false && focusFirstChild() == false)
		{
			element->setTabIndex(0);
			element->focus();
		}
		return layer;
	}

	void View::closeModal()
	{
		setParent(nullptr);
		DocumentBodyView::get()->endModal();
		delete this;
	}

	void View::setDefaultFocus()
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
}

/*
static struct JSCode { JSCode() { emscripten_run_script(R"jscode(

	class ViewElement extends HTMLElement { constructor() { super(); } };
	window.customElements.define("basic-view", ViewElement);

)jscode"); } } initJSCode;
*/
