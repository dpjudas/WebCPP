
#include <WebCPP/Core/DocumentBodyView.h>
#include <WebCPP/Controls/TextLabel/TextLabel.h>

class ApplicationView : public DocumentBodyView
{
public:
	ApplicationView()
	{
		addClass("application");
		label = new TextLabel(this);
		label->addClass("hellolabel");
		label->setText("Hello World!");
	}

	TextLabel* label = nullptr;
};

int main()
{
	DocumentBodyView::create<ApplicationView>();
	return 0;
}
