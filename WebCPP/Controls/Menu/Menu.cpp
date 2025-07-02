
#include "WebCPP/Controls/Menu/Menu.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"

namespace web
{
	Menu::Menu() : View("menu-view")
	{
		createVBoxLayout();
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
		parent()->element->addEventListener("click", [=](Event* event) { event->stopPropagation(); closeModal(); });
		closeMenu = [this]() { closeModal(); };
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
		item->element->addEventListener("click", [=](Event* event)
			{
				event->stopPropagation();
				if (closeMenu)
					closeMenu();
				if (onClick)
					onClick();
			});
		return item;
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
}
