#pragma once

#include "../../Core/View.h"

namespace web
{
	class MenuItem;
	class MenuItemSeparator;
	class ImageBox;
	class TextLabel;

	enum class MenuOpenCorner
	{
		topLeft,
		topRight,
		bottomLeft,
		bottomRight
	};

	class Menu : public View
	{
	public:
		Menu();

		void showContextMenu(double clientX, double clientY, MenuOpenCorner openCorner = MenuOpenCorner::topLeft);

		void setLeftPosition(double x, double y);
		void setRightPosition(double x, double y);
		std::shared_ptr<MenuItem> addItem(std::string icon, std::string text, std::function<void()> onClick = {});
		std::shared_ptr<MenuItemSeparator> addSeparator();
		bool hasItems() const { return itemCount > 0; }

		std::function<void()> closeMenu;

	private:
		void onParentClick(Event* event);
		void onParentContextMenu(Event* event);
		void onItemClick(MenuItem* item, std::function<void()> onClick, Event* event);

		size_t itemCount = 0;
	};

	class MenuItem : public View
	{
	public:
		MenuItem();

		void setEnabled(bool value);
		bool getEnabled() const { return enabled; }

		std::shared_ptr<ImageBox> icon;
		std::shared_ptr<TextLabel> text;

	private:
		bool enabled = true;
	};

	class MenuItemSeparator : public View
	{
	public:
		MenuItemSeparator() : View("menuitemseparator-view") {}
	};
}
