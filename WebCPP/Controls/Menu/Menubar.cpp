
#include "WebCPP/Controls/Menu/Menubar.h"
#include "WebCPP/Controls/Menu/MenubarModal.h"

namespace web
{
	Menubar::Menubar() : View("menubar-view")
	{
		spacer = std::make_shared<View>("menubarspacer-view");

		auto layout = createHBoxLayout();
		layout->addView(spacer, true, true);
	}

	std::shared_ptr<MenubarItem> Menubar::addItem(std::string text, std::function<void(Menu* menu)> onOpen, bool alignRight)
	{
		auto item = std::make_shared<MenubarItem>(this, alignRight);
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
		modal->showPopupModal();
	}
}
