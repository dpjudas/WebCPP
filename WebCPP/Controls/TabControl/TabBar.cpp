
#include "WebCPP/Controls/TabControl/TabBar.h"
#include "WebCPP/Controls/TabControl/TabBarTab.h"

namespace web
{
	TabBar::TabBar() : View("tabbar-view")
	{
		createHBoxLayout();
	}

	std::shared_ptr<TabBarTab> TabBar::addTab(const std::string& icon, const std::string& label, std::function<void()> onClick, std::function<void(double clientX, double clientY)> onContextMenu)
	{
		auto tab = std::make_shared<TabBarTab>();
		tab->setText(label);
		tab->setIcon(icon);
		tab->element->addEventListener("click", [=](Event* event) { event->stopPropagation(); if (onClick) onClick(); });
		tab->element->addEventListener("contextmenu", [=](Event* event) { event->stopPropagation(); event->preventDefault(); if (onContextMenu) onContextMenu(event->clientX(), event->clientY()); });
		getLayout<HBoxLayout>()->addView(tab);
		return tab;
	}
}
