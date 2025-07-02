
#include "WebCPP/Controls/TabControl/TabBarTab.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"

namespace web
{
	TabBarTab::TabBarTab() : View("tabbartab-view")
	{
		icon = std::make_shared<ImageBox>();
		label = std::make_shared<TextLabel>();

		icon->addClass("tabbartab-icon");
		label->addClass("tabbartab-label");

		auto layout = createHBoxLayout();
		layout->addView(icon);
		layout->addView(label);
	}

	void TabBarTab::setText(const std::string& text)
	{
		label->setText(text);
	}

	void TabBarTab::setIcon(const std::string& src)
	{
		icon->setSrc(src);
	}
}
