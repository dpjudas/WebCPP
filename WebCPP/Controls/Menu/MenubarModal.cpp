
#include "WebCPP/Controls/Menu/MenubarModal.h"
#include "WebCPP/Controls/Menu/Menubar.h"
#include "WebCPP/Controls/Menu/Menu.h"

namespace web
{
	MenubarModal::MenubarModal(Menubar* menubar, MenubarItem* openitem) : View("menubarmodal-view"), menubar(menubar)
	{
		spacer = std::make_shared<View>("menubarmodalspacer-view");

		auto layout = createHBoxLayout();
		layout->addView(spacer, true, true);

		for (std::shared_ptr<MenubarItem> src : menubar->menuItems)
		{
			auto item = std::make_shared<MenubarModalItem>(this, src->alignRight);
			item->addClass("menubarmodal-item");
			item->setText(src->getText());
			item->setOpenCallback(src->getOpenCallback());
			if (src.get() == openitem)
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
		if (openMenu)
			openMenu->detach();
		openMenu = nullptr;

		for (auto i : items)
		{
			if (i.get() == item)
			{
				i->open();
			}
			else
			{
				i->close();
			}
		}

		Rect box = item->element->getBoundingClientRect();
		openMenu = std::make_shared<Menu>();
		openMenu->closeMenu = std::bind_front(&MenubarModal::closeModal, this);
		openMenu->addClass("menubarmodal-openmenu");
		if (item->alignRight)
			openMenu->setRightPosition(element->getBoundingClientRect().width - box.x - box.width, box.y + box.height - 1);
		else
			openMenu->setLeftPosition(box.x, box.y + box.height - 1);
		if (item->getOpenCallback())
		{
			item->getOpenCallback()(openMenu.get());
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
		parent()->element->addEventListener("click", std::bind_front(&MenubarModal::onClose, this));
		if (firstOpenMenuItem)
		{
			showMenu(firstOpenMenuItem.get());
			firstOpenMenuItem = nullptr;
		}
	}

	/////////////////////////////////////////////////////////////////////////////

	MenubarModalItem::MenubarModalItem(MenubarModal* menubar, bool alignRight) : menubar(menubar), alignRight(alignRight)
	{
		addClass("menubarmodalitem");
		element->addEventListener("click", std::bind_front(&MenubarModalItem::onClick, this));
		element->addEventListener("mouseenter", std::bind_front(&MenubarModalItem::onMouseEnter, this));
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
}
