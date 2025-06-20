#pragma once

#include "../../Core/View.h"
#include "../../Controls/TextLabel/TextLabel.h"

namespace web
{
	class Menu;
	class MenubarItem;

	class Menubar : public View
	{
	public:
		Menubar();

		MenubarItem* addItem(std::string text, std::function<void(Menu* menu)> onOpen, bool alignRight = false);

	private:
		View* spacer = nullptr;
		std::vector<MenubarItem*> menuItems;

		friend class MenubarModal;
	};

	class MenubarItem : public TextLabel
	{
	public:
		MenubarItem(Menubar* menubar, bool alignRight);

		void setOpenCallback(std::function<void(Menu* menu)> callback) { onOpen = std::move(callback); }
		const std::function<void(Menu* menu)>& getOpenCallback() const { return onOpen; }

		bool alignRight = false;

	private:
		void onClick(Event* event);

		Menubar* menubar = nullptr;
		std::function<void(Menu* menu)> onOpen;
	};
}
