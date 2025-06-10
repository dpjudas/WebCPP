
#include "Menu.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"

Menu::Menu() : View("menu-view")
{
	createVBoxLayout();
}

void Menu::showContextMenu(double clientX, double clientY, MenuOpenCorner openCorner)
{
	showUnshadedModal();
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

MenuItem* Menu::addItem(std::string icon, std::string text, std::function<void()> onClick)
{
	auto item = new MenuItem();
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

MenuItemSeparator* Menu::addSeparator()
{
	auto sep = new MenuItemSeparator();
	sep->addClass("menu-sep");
	getLayout<VBoxLayout>()->addView(sep);
	return sep;
}

/////////////////////////////////////////////////////////////////////////////

MenuItem::MenuItem() : View("menuitem-view")
{
	icon = new ImageBox();
	text = new TextLabel();

	icon->addClass("menuitem-icon");
	text->addClass("menuitem-text");

	auto layout = createHBoxLayout();
	layout->addView(icon);
	layout->addView(text);
}
