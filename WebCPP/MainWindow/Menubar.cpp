
#include "Menubar.h"
#include "MenubarModal.h"

Menubar::Menubar(View* parent) : HBoxView(parent)
{
	addClass("menubar");
	spacer = new View(this);
	spacer->addClass("menubar-spacer");
}

MenubarItem* Menubar::addItem(std::string text, std::function<void(Menu* menu)> onOpen, bool alignRight)
{
	auto item = new MenubarItem(this, alignRight);
	if (!alignRight)
		item->moveBefore(spacer);
	item->addClass("menubar-item");
	item->setText(text);
	item->setOpenCallback(std::move(onOpen));
	menuItems.push_back(item);
	return item;
}

/////////////////////////////////////////////////////////////////////////////

MenubarItem::MenubarItem(Menubar* menubar, bool alignRight) : TextLabel(menubar), menubar(menubar), alignRight(alignRight)
{
	addClass("menubaritem");
	element->addEventListener("click", [=](Event* event) { onClick(event); });
	element->addEventListener("mouseenter", [=](Event* event) { onMouseEnter(event); });
	element->addEventListener("mouseleave", [=](Event* event) { onMouseLeave(event); });
}

void MenubarItem::onClick(Event* event)
{
	event->stopPropagation();
	auto modal = new MenubarModal(menubar, this);
	modal->showModal();
}

void MenubarItem::onMouseEnter(Event* event)
{
	element->setStyle("background", "rgb(23,107,209)");
}

void MenubarItem::onMouseLeave(Event* event)
{
	element->setStyle("background", "none");
}
