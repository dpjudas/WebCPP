#pragma once

#include "../../Core/View.h"

namespace web
{
	class TabBar;
	class TabBarTab;

	class TabControl : public View
	{
	public:
		TabControl(bool tabsAtBottom = false);

		void addPage(std::string icon, std::string label, View* page, std::function<void(double clientX, double clientY)> onContextMenu = {});
		void showPage(View* page);

		std::function<void(View*)> onPageShow;

	private:
		TabBarTab* findTab(View* page);
		void onPageTabClicked(View* page);
		void setupUi();

		bool tabsAtBottom = false;
		TabBar* tabs = nullptr;
		std::map<TabBarTab*, std::unique_ptr<View>> pages;
		View* currentPage = nullptr;

		View* widgetStack = nullptr;
	};
}
