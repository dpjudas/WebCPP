
#include "WebPageFooter.h"
#include <WebCPP/Controls/TextLabel/TextLabel.h>

WebPageFooter::WebPageFooter(View* parent) : VBoxView(parent)
{
	addClass("webpagefooter");

	auto content = new VBoxView(this);
	content->addClass("webpagefootercontent");

	auto footertext = new TextLabel(content);
	footertext->setText("Footer stuff goes here");

	copyright = new TextLabel(this);
	copyright->addClass("webpagecopyright");
	copyright->setHtml("Copyright &copy; WebCPP");
}
