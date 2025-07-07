#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <functional>
#include "Element.h"
#include "ViewLayout.h"

namespace web
{
	class ModalLayer;
	class ShadowRoot;

	class View : public std::enable_shared_from_this<View>
	{
	public:
		View(std::string elementType = "basic-view");
		View(std::string ns, std::string elementType);
		View(std::unique_ptr<Element> element);
		virtual ~View();

		void detach();

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

		std::shared_ptr<ModalLayer> showDialogModal(bool setFocus = true);
		std::shared_ptr<ModalLayer> showPopupModal(bool setFocus = true);
		void closeModal();

		void attachShadow(const std::string& mode = "open");

		virtual void setDefaultFocused();
		virtual bool setFocus();
		virtual void onModalAttach() {}

		bool applyDefaultFocus();

		std::unique_ptr<Element> element;
		std::unique_ptr<ShadowRoot> shadowRoot;

		View* parent() const;

		void setCursor(const std::string& cursor) { element->setAttribute("cursor", cursor); }
		void setPointerEvents(const std::string& value) { element->setAttribute("pointer-events", value); }

	private:
		void updateClassAttribute();
		bool focusFirstChild();

		std::unique_ptr<ViewLayout> layout;
		ViewLayoutItem* layoutItem = nullptr;

		std::set<std::string> classes;

		bool defaultFocused = false;

		friend class ViewLayout;
	};
}
