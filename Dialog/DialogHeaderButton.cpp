
#include "DialogHeaderButton.h"

DialogHeaderButton::DialogHeaderButton(View* parent, std::string svg) : ImageBox(parent)
{
	addClass("dialogheaderbutton");
	setSrc("data:image/svg+xml;base64, " + svg);
}
