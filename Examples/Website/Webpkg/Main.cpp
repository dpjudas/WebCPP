
#include <WebCPP/Core/DocumentBodyView.h>
#include <WebCPP/Controls/TextLabel/TextLabel.h>
#include <WebCPP/Dialog/WebsiteHeader.h>

class ApplicationView : public DocumentBodyView
{
public:
	ApplicationView()
	{
		//addClass("application");
		addClass("webpage");

		header = new WebsiteHeader(this);
		header->setIcon("/Resources/Icons/appmenu.svg");
		header->setTitle("WebCpp website example");
		header->setAccountMenuText("Account");

		label = new TextLabel(this);
		label->addClass("hellolabel");
		label->setText("Hello World!");
	}

	WebsiteHeader* header = nullptr;
	TextLabel* label = nullptr;
};

int main()
{
	DocumentBodyView::create<ApplicationView>();
	return 0;
}
