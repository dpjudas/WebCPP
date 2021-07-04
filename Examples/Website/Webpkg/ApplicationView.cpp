
#include "ApplicationView.h"
#include "WebPageHeader.h"
#include "WebPageBody.h"
#include "WebPageFooter.h"

ApplicationView::ApplicationView()
{
	addClass("application");

	header = new WebPageHeader(this);
	page = new WebPageBody(this);
	footer = new WebPageFooter(this);
}
