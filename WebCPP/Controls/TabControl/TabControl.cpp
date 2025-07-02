
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

		tabs = std::make_shared<TabBar>();

		widgetStack = std::make_shared<View>();
		widgetStack->addClass("tabcontrol-widgetstack");
		widgetStack->createVBoxLayout();

		auto layout = createVBoxLayout();
		if (!tabsAtBottom)
			layout->addView(tabs);
		layout->addView(widgetStack, true, true);
		if (tabsAtBottom)
			layout->addView(tabs);
	}

	void TabControl::addPage(std::string icon, std::string label, std::shared_ptr<View> page, std::function<void(double clientX, double clientY)> onContextMenu)
	{
		std::shared_ptr<TabBarTab> tab = tabs->addTab(icon, label, [=]() { onPageTabClicked(page); }, onContextMenu);

		pages[tab] = page;
		if (!currentPage)
			showPage(page);
	}

	void TabControl::showPage(std::shared_ptr<View> page)
	{
		if (page != currentPage)
		{
			if (currentPage)
			{
				findTab(currentPage.get())->removeClass("selected");
				currentPage->detach();
			}
			currentPage = page;
			if (currentPage)
			{
				currentPage->detach();

				auto layout = widgetStack->createVBoxLayout();
				layout->addView(currentPage, true, true);

				findTab(currentPage.get())->addClass("selected");
			}
		}
	}

	void TabControl::onPageTabClicked(std::shared_ptr<View> page)
	{
		showPage(page);
		if (onPageShow)
			onPageShow(page.get());
	}

	std::shared_ptr<TabBarTab> TabControl::findTab(View* page)
	{
		for (auto& it : pages)
		{
			if (it.second.get() == page)
				return it.first;
		}
		return nullptr;
	}
}
