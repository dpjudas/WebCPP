
#include "WebCPP/Controls/Menu/Menu.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"
#include "WebCPP/Core/HtmlDocument.h"
#include "WebCPP/Core/Timer.h"

#include <algorithm>

namespace web
{
	Menu::Menu() : View("menu-view")
	{
		createVBoxLayout();
	}

	Menu::~Menu()
	{
		if (hideSubMenuTimeoutID != -1)
			clearTimeout(hideSubMenuTimeoutID);
	}

	void Menu::showContextMenu(double clientX, double clientY, MenuOpenCorner openCorner)
	{
		showPopupModal();
		if (openCorner == MenuOpenCorner::topLeft)
		{
			element->setStyle("left", std::to_string(clientX) + "px");
			element->setStyle("top", std::to_string(clientY) + "px");
		}
		else if (openCorner == MenuOpenCorner::topRight)
		{
			element->setStyle("right", std::to_string(clientX) + "px");
			element->setStyle("top", std::to_string(clientY) + "px");
		}
		else if (openCorner == MenuOpenCorner::bottomLeft)
		{
			element->setStyle("left", std::to_string(clientX) + "px");
			element->setStyle("bottom", "calc(100vh - " + std::to_string(clientY) + "px)");
		}
		else if (openCorner == MenuOpenCorner::bottomRight)
		{
			element->setStyle("right", std::to_string(clientX) + "px");
			element->setStyle("bottom", "calc(100vh - " + std::to_string(clientY) + "px)");
		}
		parent()->element->addEventListener("click", std::bind_front(&Menu::onParentClick, this));
		parent()->element->addEventListener("contextmenu", std::bind_front(&Menu::onParentContextMenu, this));
		closeMenu = std::bind_front(&Menu::closeModal, this);
	}

	void Menu::onParentClick(Event* event)
	{
		event->stopPropagation();
		closeModal();
	}

	void Menu::onParentContextMenu(Event* event)
	{
		event->stopPropagation();
		event->preventDefault();
		closeModal();
	}

	void Menu::setLeftPosition(double x, double y)
	{
		element->setStyle("left", std::to_string(x) + "px");
		element->setStyle("top", std::to_string(y) + "px");
	}

	void Menu::setRightPosition(double x, double y)
	{
		element->setStyle("right", std::to_string(x) + "px");
		element->setStyle("top", std::to_string(y) + "px");
	}

	std::shared_ptr<MenuItem> Menu::addItem(std::string icon, std::string text, std::function<void()> onClick)
	{
		auto item = std::make_shared<MenuItem>();
		getLayout<VBoxLayout>()->addView(item);
		item->addClass("menu-item");
		if (!icon.empty())
			item->icon->setSrc(icon);
		item->text->setText(text);
		item->element->addEventListener("click", std::bind_front(&Menu::onItemClick, this, item.get(), onClick));
		if (!firstItem)
			firstItem = item.get();
		itemCount++;
		return item;
	}

	std::shared_ptr<MenuItem> Menu::addSubMenu(std::string icon, std::string text)
	{
		std::shared_ptr<MenuItem> item = addItem(icon, text);
		item->addClass("menuitem-hassubmenu");

		auto arrow = std::make_shared<TextLabel>("›");
		arrow->addClass("menuitem-submenuarrow");
		item->getLayout<HBoxLayout>()->addView(arrow);

		item->subMenu = std::make_shared<Menu>();
		item->subMenu->parentMenu = this;
		item->subMenu->addClass("submenu");
		item->subMenu->hide();
		item->getLayout<HBoxLayout>()->addView(item->subMenu);

		item->element->addEventListener("mouseenter", std::bind_front(&Menu::onSubMenuEnter, this, item.get()));
		item->element->addEventListener("mouseleave", std::bind_front(&Menu::onSubMenuLeave, this, item.get()));
		return item;
	}

	void Menu::onSubMenuEnter(MenuItem* item, Event* event)
	{
		if (hideSubMenuTimeoutID != -1)
		{
			clearTimeout(hideSubMenuTimeoutID);
			hideSubMenuTimeoutID = -1;
		}
		if (openSubMenuItem == item)
			return;
		hideSubMenu();

		if (!item->getEnabled() || !item->subMenu->hasItems())
			return;

		Menu* sub = item->subMenu.get();
		sub->show();
		openSubMenuItem = item;

		Rect itemRect = item->element->getBoundingClientRect();
		Rect menuRect = element->getBoundingClientRect();
		Rect subRect = sub->element->getBoundingClientRect();
		double viewportWidth = HtmlDocument::body()->element->clientWidth();
		double viewportHeight = HtmlDocument::body()->element->clientHeight();

		double innerLeft = menuRect.x + element->clientLeft();
		double innerRight = innerLeft + element->clientWidth();
		double left = innerRight;
		if (left + subRect.width > viewportWidth)
			left = std::max(0.0, innerLeft - subRect.width);
		double top = itemRect.y - (sub->firstItem->element->offsetTop() + sub->element->clientTop());
		if (top + subRect.height > viewportHeight)
			top = std::max(0.0, viewportHeight - subRect.height);
		sub->setLeftPosition(left, top);
	}

	void Menu::onSubMenuLeave(MenuItem* item, Event* event)
	{
		if (openSubMenuItem != item)
			return;
		if (hideSubMenuTimeoutID != -1)
			clearTimeout(hideSubMenuTimeoutID);
		hideSubMenuTimeoutID = setTimeout([this]() {
			hideSubMenuTimeoutID = -1;
			hideSubMenu();
		}, 300);
	}

	void Menu::hideSubMenu()
	{
		if (openSubMenuItem)
		{
			openSubMenuItem->subMenu->hide();
			openSubMenuItem = nullptr;
		}
	}

	void Menu::onItemClick(MenuItem* item, std::function<void()> onClick, Event* event)
	{
		event->stopPropagation();
		if (!item->getEnabled() || item->subMenu)
			return;

		// Items of a submenu close the whole menu, not just their own level
		Menu* root = this;
		while (root->parentMenu)
			root = root->parentMenu;
		if (root->closeMenu)
			root->closeMenu();
		if (onClick)
			onClick();
	}

	std::shared_ptr<MenuItemSeparator> Menu::addSeparator()
	{
		auto sep = std::make_shared<MenuItemSeparator>();
		sep->addClass("menu-sep");
		getLayout<VBoxLayout>()->addView(sep);
		return sep;
	}

	/////////////////////////////////////////////////////////////////////////////

	MenuItem::MenuItem() : View("menuitem-view")
	{
		icon = std::make_shared<ImageBox>();
		text = std::make_shared<TextLabel>();

		icon->addClass("menuitem-icon");
		text->addClass("menuitem-text");

		auto layout = createHBoxLayout();
		layout->addView(icon);
		layout->addView(text);
	}

	void MenuItem::setEnabled(bool value)
	{
		if (enabled != value)
		{
			enabled = value;
			if (enabled)
				removeClass("disabled");
			else
				addClass("disabled");
		}
	}
}
