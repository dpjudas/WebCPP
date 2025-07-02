#pragma once

#include "../../Core/View.h"
#include "../../Controls/TextLabel/TextLabel.h"

namespace web
{
	class Menu;
	class Menubar;
	class MenubarItem;
	class MenubarModalItem;

	class MenubarModal : public View
	{
	public:
		MenubarModal(Menubar* menubar, MenubarItem* openitem);

		void updateLayout();
		void itemClicked(MenubarModalItem* item);
		void itemMouseEnter(MenubarModalItem* item);
		void showMenu(MenubarModalItem* item);
		void onClose(Event* event);
		void onModalAttach() override;

	private:
		Menubar* menubar = nullptr;
		std::shared_ptr<Menu> openMenu;
		std::shared_ptr<MenubarModalItem> firstOpenMenuItem;
		std::shared_ptr<View> spacer;
		std::vector<std::shared_ptr<MenubarModalItem>> items;
	};

	class MenubarModalItem : public TextLabel
	{
	public:
		MenubarModalItem(MenubarModal* menubar, bool alignRight);

		void setOpenCallback(std::function<void(Menu* menu)> callback) { onOpen = std::move(callback); }
		const std::function<void(Menu* menu)>& getOpenCallback() const { return onOpen; }

		void open();
		void close();

		bool alignRight = false;

	private:
		void onClick(Event* event);
		void onMouseEnter(Event* event);

		MenubarModal* menubar = nullptr;
		std::function<void(Menu* menu)> onOpen;
	};
}
