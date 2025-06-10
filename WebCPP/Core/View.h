#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <functional>
#include <emscripten.h>
#include <emscripten/val.h>
#include "Element.h"
#include "ViewLayout.h"

class ModalLayer;
class ShadowRoot;

class View
{
public:
	View(std::string elementType = "basic-view");
	View(std::string ns, std::string elementType);
	View(std::unique_ptr<Element> element);
	virtual ~View();

	void setParent(View* parent);

	void addClass(std::string name);
	void removeClass(std::string name);
	bool hasClass(std::string name) const;

	template<typename T> T* createLayout() { layout = std::make_unique<T>(this); return getLayout<T>(); }
	FlowLayout* createFlowLayout() { return createLayout<FlowLayout>(); }
	FlexLayout* createFlexLayout() { return createLayout<FlexLayout>(); }
	VBoxLayout* createVBoxLayout() { return createLayout<VBoxLayout>(); }
	HBoxLayout* createHBoxLayout() { return createLayout<HBoxLayout>(); }
	GridLayout* createGridLayout() { return createLayout<GridLayout>(); }

	ViewLayout* getLayout() { return layout.get(); }
	template<typename T> T* getLayout() { return static_cast<T*>(layout.get()); }

	void show() { setVisible(true); }
	void hide() { setVisible(false); }
	bool getVisible() const { return !getHidden(); }
	void setVisible(bool value) { setHidden(!value); }
	bool getHidden() const;
	void setHidden(bool value);

	ModalLayer* showModal();
	ModalLayer* showUnshadedModal(const bool setFocus = true);
	void closeModal();

	void attachShadow(const std::string& mode = "open");

	virtual void setDefaultFocus();
	virtual bool setFocus();
	virtual void onModalAttach() { }

	std::unique_ptr<Element> element;
	std::unique_ptr<ShadowRoot> shadowRoot;

	View* parent() const { return parentObj; }

	void setCursor(const std::string& cursor) { element->setAttribute("cursor", cursor); }
	void setPointerEvents(const std::string& value) { element->setAttribute("pointer-events", value); }

private:
	View* prevSibling() const { return prevSiblingObj; }
	View* nextSibling() const { return nextSiblingObj; }
	View* firstChild() const { return firstChildObj; }
	View* lastChild() const { return lastChildObj; }

	bool forceFocus();
	bool focusFirstChild();
	void detachFromParent(bool notifyLayout);
	void updateClassAttribute();

	View* parentObj = nullptr;
	View* prevSiblingObj = nullptr;
	View* nextSiblingObj = nullptr;
	View* firstChildObj = nullptr;
	View* lastChildObj = nullptr;

	std::unique_ptr<ViewLayout> layout;
	std::unique_ptr<ViewLayoutItem> layoutItem;

	std::set<std::string> classes;

	bool defaultFocused = false;

	friend class ViewLayout;
	friend class SvgElementView;
};
