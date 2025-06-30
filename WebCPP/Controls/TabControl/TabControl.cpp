
#include "WebCPP/Controls/TabControl/TabControl.h"
#include "WebCPP/Controls/TabControl/TabBar.h"
#include "WebCPP/Controls/TabControl/TabBarTab.h"

namespace web
{
	TabControl::TabControl(bool tabsAtBottom) : View("tabcontrol-view"), tabsAtBottom(tabsAtBottom)
	{
		setupUi();
	}

	void TabControl::setupUi()
	{
		if (tabsAtBottom)
			addClass("tabsAtBottom");

		tabs = new TabBar();

		widgetStack = new View();
		widgetStack->addClass("tabcontrol-widgetstack");
		widgetStack->createVBoxLayout();

		auto layout = createVBoxLayout();
		if (!tabsAtBottom)
			layout->addView(tabs);
		layout->addView(widgetStack, true, true);
		if (tabsAtBottom)
			layout->addView(tabs);
	}

	void TabControl::addPage(std::string icon, std::string label, View* page, std::function<void(double clientX, double clientY)> onContextMenu)
	{
		TabBarTab* tab = tabs->addTab(icon, label, [=]() { onPageTabClicked(page); }, onContextMenu);

		pages[tab] = std::unique_ptr<View>(page);
		if (!currentPage)
			showPage(page);
	}

	void TabControl::showPage(View* page)
	{
		if (page != currentPage)
		{
			if (currentPage)
			{
				findTab(currentPage)->removeClass("selected");
				currentPage->detach();
			}
			currentPage = page;
			if (currentPage)
			{
				currentPage->detach();

				auto layout = widgetStack->createVBoxLayout();
				layout->addView(currentPage, true, true);

				findTab(currentPage)->addClass("selected");
			}
		}
	}

	void TabControl::onPageTabClicked(View* page)
	{
		showPage(page);
		if (onPageShow)
			onPageShow(page);
	}

	TabBarTab* TabControl::findTab(View* page)
	{
		for (auto& it : pages)
		{
			if (it.second.get() == page)
				return it.first;
		}
		return nullptr;
	}
}
