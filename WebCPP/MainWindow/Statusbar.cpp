
#include "Statusbar.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"

Statusbar::Statusbar(View* parent) : HBoxView(parent)
{
	addClass("statusbar");
}

StatusbarItem* Statusbar::addItem(std::string text)
{
	auto item = new StatusbarItem(this);
	item->text->setText(text);
	return item;
}

/////////////////////////////////////////////////////////////////////////////

StatusbarItem::StatusbarItem(View* parent) : HBoxView(parent)
{
	text = new TextLabel(this);

	addClass("statusbaritem");
	text->addClass("statusbaritem-text");
}
