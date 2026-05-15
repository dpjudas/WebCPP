
#include "WebCPP/Controls/ListView/ListView.h"
#include "WebCPP/Controls/ListView/ListViewItem.h"
#include "WebCPP/Controls/ListView/ListViewHeader.h"
#include "WebCPP/Controls/ListView/ListViewBody.h"

namespace web
{
	ListViewBody::ListViewBody() : View("listviewbody-view")
	{
		auto layout = createGridLayout();
		layout->setGap(0.0, 0.0);
	}

	void ListViewBody::updateColumns(ListViewHeader* header)
	{
		size_t count = header->getColumnCount();

		size_t lastVisible = count;
		for (size_t i = 0; i < count; i++)
		{
			if (header->getColumnWidth(i) > 0.0 || header->isColumnExpanding(i))
				lastVisible = i;
		}

		std::vector<GridTrackSize> sizes;
		for (size_t i = 0; i < count; i++)
		{
			double width = header->getColumnWidth(i);
			double gap = (lastVisible < count && i < lastVisible) ? 10.0 : 0.0;

			if (header->isColumnExpanding(i))
				sizes.push_back(GridLayout::minmaxSize(width + gap, GridLayout::autoSize));
			else
				sizes.push_back(width + gap);
		}

		getLayout<GridLayout>()->setColumns(sizes);
	}

	void ListViewBody::addViewBefore(std::shared_ptr<ListViewItemView> view, std::shared_ptr<ListViewItemView> sibling, ListViewHeader* header)
	{
		getLayout<GridLayout>()->addViewBefore(view, sibling, 1, 0, header->getColumnCount());
	}
}
