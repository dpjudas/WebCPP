
#include "Statusbar.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"

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
	icon = new ImageBox(this);
	text = new TextLabel(this);

	addClass("statusbaritem");
	icon->addClass("statusbaritem-icon");
	text->addClass("statusbaritem-text");
}
