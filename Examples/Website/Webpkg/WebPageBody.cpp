
#include "WebPageBody.h"
#include "WebPageBlock.h"
#include "BigTeaser.h"
#include "ThreeChoices.h"
#include "TwoChoices.h"
#include <WebCPP/Controls/TextLabel/TextLabel.h>

WebPageBody::WebPageBody(View* parent) : VBoxView(parent)
{
	addClass("webpagebody");

	auto teaser1 = new BigTeaser(this);
	teaser1->setImage("/Resources/Images/image1.jpg");
	teaser1->setSection("Dogs");
	teaser1->setHeadline("Now that's a cute dog! Let's add some more text so that it wordwraps. Blah blah blah and more blah");

	auto choices2 = new TwoChoices(this);
	choices2->headline->setText("We have two amazing choices for you");
	choices2->left->setImage("/Resources/Images/image3.jpg");
	choices2->left->setSection("Forests");
	choices2->left->setHeadline("Robin Hood seen lurking around in this forest! Maybe Picard is there too somehow");
	choices2->right->setImage("/Resources/Images/image5.jpg");
	choices2->right->setSection("Cities");
	choices2->right->setHeadline("Another city from somewhere in Europe if I'm to guess");

	auto teaser2 = new BigTeaser(this);
	teaser2->setImage("/Resources/Images/image2.jpg");
	teaser2->setSection("Plant life");
	teaser2->setHeadline("Water on what appears to be a leaf. Looks great!");

	auto choices = new ThreeChoices(this);
	choices->left->setImage("/Resources/Images/image2.jpg");
	choices->left->setSection("Plant life");
	choices->left->setHeadline("Still that same watery leaf!");
	choices->center->setImage("/Resources/Images/image1.jpg");
	choices->center->setSection("Dogs");
	choices->center->setHeadline("The cute dog is back");
	choices->right->setImage("/Resources/Images/image3.jpg");
	choices->right->setSection("Forests");
	choices->right->setHeadline("Maybe I should have downloaded more cc0 pictures");

	auto teaser3 = new BigTeaser(this);
	teaser3->setImage("/Resources/Images/image4.jpg");
	teaser3->setSection("Cats");
	teaser3->setHeadline("If you're not into dogs we also have cats! Blah blah blah and more blah. Let's add some more text so that it wordwraps");
}
