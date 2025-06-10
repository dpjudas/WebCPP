
#include "Menubar.h"
#include "MenubarModal.h"

Menubar::Menubar() : View("menubar-view")
{
	spacer = new View("menubarspacer-view");

	auto layout = createHBoxLayout();
	layout->addView(spacer, true, true);
}

MenubarItem* Menubar::addItem(std::string text, std::function<void(Menu* menu)> onOpen, bool alignRight)
{
	auto item = new MenubarItem(this, alignRight);
	item->addClass("menubar-item");
	item->setText(text);
	item->setOpenCallback(std::move(onOpen));

	getLayout<HBoxLayout>()->addViewBefore(item, !alignRight ? spacer : nullptr);

	menuItems.push_back(item);
	return item;
}

/////////////////////////////////////////////////////////////////////////////

MenubarItem::MenubarItem(Menubar* menubar, bool alignRight) : menubar(menubar), alignRight(alignRight)
{
	addClass("menubaritem");
	element->addEventListener("click", [=](Event* event) { onClick(event); });
}

void MenubarItem::onClick(Event* event)
{
	event->stopPropagation();
	auto modal = new MenubarModal(menubar, this);
	modal->showModal();
}
