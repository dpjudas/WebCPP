
#include "WebCPP/Controls/Dialog/DialogHeaderButton.h"

namespace web
{
	DialogHeaderButton::DialogHeaderButton(std::string svg)
	{
		addClass("dialogheaderbutton");
		setSrc("data:image/svg+xml;base64, " + svg);
	}
}
