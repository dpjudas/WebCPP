
#include "WebPageHeader.h"
#include <WebCPP/Controls/TextLabel/TextLabel.h>

WebPageHeader::WebPageHeader(View* parent) : HBoxView(parent)
{
	addClass("webpageheader");

	auto text = new TextLabel(this);
	text->setText("WebCPP website example");
}
