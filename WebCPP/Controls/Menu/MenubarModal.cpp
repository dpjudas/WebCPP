
#include "MenubarModal.h"
#include "Menubar.h"
#include "Menu.h"

MenubarModal::MenubarModal(Menubar* menubar, MenubarItem* openitem) : View("menubarmodal-view"), menubar(menubar)
{
	spacer = new View("menubarmodalspacer-view");

	auto layout = createHBoxLayout();
	layout->addView(spacer, true, true);

	for (MenubarItem* src : menubar->menuItems)
	{
		auto item = new MenubarModalItem(this, src->alignRight);
		item->addClass("menubarmodal-item");
		item->setText(src->getText());
		item->setOpenCallback(src->getOpenCallback());
		if (src == openitem)
		{
			firstOpenMenuItem = item;
		}
		layout->addViewBefore(item, !item->alignRight ? spacer : nullptr);
		items.push_back(item);
	}

	updateLayout();
}

void MenubarModal::updateLayout()
{
	Rect box = menubar->element->getBoundingClientRect();
	element->setStyle("position", "absolute");
	element->setStyle("left", std::to_string(box.x) + "px");
	element->setStyle("top", std::to_string(box.y) + "px");
	element->setStyle("width", std::to_string(box.width) + "px");
	element->setStyle("height", std::to_string(box.height) + "px");
}

void MenubarModal::itemClicked(MenubarModalItem* item)
{
}

void MenubarModal::itemMouseEnter(MenubarModalItem* item)
{
	showMenu(item);
}

void MenubarModal::showMenu(MenubarModalItem* item)
{
	delete openMenu;
	openMenu = nullptr;

	for (auto i : items)
	{
		if (i == item)
		{
			i->open();
		}
		else
		{
			i->close();
		}
	}

	Rect box = item->element->getBoundingClientRect();
	openMenu = new Menu();
	openMenu->closeMenu = [=]() { closeModal(); };
	openMenu->addClass("menubarmodal-openmenu");
	if (item->alignRight)
		openMenu->setRightPosition(element->getBoundingClientRect().width - box.x - box.width, box.y + box.height - 1);
	else
		openMenu->setLeftPosition(box.x, box.y + box.height - 1);
	if (item->getOpenCallback())
	{
		item->getOpenCallback()(openMenu);
	}

	getLayout<HBoxLayout>()->addView(openMenu);
}

void MenubarModal::onClose(Event* event)
{
	event->stopPropagation();
	closeModal();
}

void MenubarModal::onModalAttach()
{
	View::onModalAttach();
	parent()->element->addEventListener("click", [=](Event* event) { onClose(event); });
	if (firstOpenMenuItem)
	{
		showMenu(firstOpenMenuItem);
		firstOpenMenuItem = nullptr;
	}
}

/////////////////////////////////////////////////////////////////////////////

MenubarModalItem::MenubarModalItem(MenubarModal* menubar, bool alignRight) : menubar(menubar), alignRight(alignRight)
{
	addClass("menubarmodalitem");
	element->addEventListener("click", [=](Event* event) { onClick(event); });
	element->addEventListener("mouseenter", [=](Event* event) { onMouseEnter(event); });
}

void MenubarModalItem::open()
{
	element->setStyle("zIndex", "1");
}

void MenubarModalItem::close()
{
	element->setStyle("zIndex", "0");
}

void MenubarModalItem::onClick(Event* event)
{
	event->stopPropagation();
	menubar->itemClicked(this);
}

void MenubarModalItem::onMouseEnter(Event* event)
{
	menubar->itemMouseEnter(this);
}
