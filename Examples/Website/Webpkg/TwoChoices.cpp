
#include "TwoChoices.h"
#include <WebCPP/Controls/ImageBox/ImageBox.h>
#include <WebCPP/Controls/TextLabel/TextLabel.h>

TwoChoices::TwoChoices(View* parent) : WebPageBlock(parent)
{
	headline = new TextLabel(this);
	list = new HBoxView(this);
	left = new TwoChoicesTeaser(list);
	right = new TwoChoicesTeaser(list);

	addClass("twochoices");
	headline->addClass("twochoices-headline");
	list->addClass("twochoices-list");
	left->addClass("twochoices-left");
	right->addClass("twochoices-right");
}

/////////////////////////////////////////////////////////////////////////////

TwoChoicesTeaser::TwoChoicesTeaser(View* parent) : VBoxView(parent)
{
	image = new ImageBox(this);
	textblock = new VBoxView(this);
	section = new TextLabel(textblock);
	headline = new TextLabel(textblock);

	addClass("twochoicesteaser");
	image->addClass("twochoicesteaser-image");
	textblock->addClass("twochoicesteaser-textblock");
	section->addClass("twochoicesteaser-section");
	headline->addClass("twochoicesteaser-headline");
}

void TwoChoicesTeaser::setImage(std::string src)
{
	image->setSrc(src);
}

void TwoChoicesTeaser::setSection(std::string text)
{
	section->setText(text);
}

void TwoChoicesTeaser::setHeadline(std::string text)
{
	headline->setText(text);
}
