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

		void addPage(std::string icon, std::string label, std::shared_ptr<View> page, std::function<void(double clientX, double clientY)> onContextMenu = {});
		void showPage(std::shared_ptr<View> page);

		std::function<void(View*)> onPageShow;

	private:
		std::shared_ptr<TabBarTab> findTab(View* page);
		void onPageTabClicked(std::shared_ptr<View> page);
		void setupUi();

		bool tabsAtBottom = false;
		std::shared_ptr<TabBar> tabs;
		std::map<std::shared_ptr<TabBarTab>, std::shared_ptr<View>> pages;
		std::shared_ptr<View> currentPage;

		std::shared_ptr<View> widgetStack;
	};
}
