
#include "BigTeaser.h"
#include <WebCPP/Controls/ImageBox/ImageBox.h>
#include <WebCPP/Controls/TextLabel/TextLabel.h>

BigTeaser::BigTeaser(View* parent) : WebPageBlock(parent)
{
	container = new VBoxView(this);
	image = new ImageBox(container);
	textblock = new VBoxView(container);
	section = new TextLabel(textblock);
	headline = new TextLabel(textblock);

	addClass("bigteaser");
	container->addClass("bigteaser-container");
	image->addClass("bigteaser-image");
	textblock->addClass("bigteaser-textblock");
	section->addClass("bigteaser-section");
	headline->addClass("bigteaser-headline");
}

void BigTeaser::setImage(std::string src)
{
	image->setSrc(src);
}

void BigTeaser::setSection(std::string text)
{
	section->setText(text);
}

void BigTeaser::setHeadline(std::string text)
{
	headline->setText(text);
}
