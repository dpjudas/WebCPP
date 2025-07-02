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

		std::function<void()> closeMenu;
	};

	class MenuItem : public View
	{
	public:
		MenuItem();

		std::shared_ptr<ImageBox> icon;
		std::shared_ptr<TextLabel> text;
	};

	class MenuItemSeparator : public View
	{
	public:
		MenuItemSeparator() : View("menuitemseparator-view")
		{
		}
	};
}
