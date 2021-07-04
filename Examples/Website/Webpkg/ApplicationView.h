#pragma once

#include <WebCPP/Core/DocumentBodyView.h>

class WebPageHeader;
class WebPageBody;
class WebPageFooter;

class ApplicationView : public DocumentBodyView
{
public:
	ApplicationView();

	WebPageHeader* header = nullptr;
	WebPageBody* page = nullptr;
	WebPageFooter* footer = nullptr;
};
