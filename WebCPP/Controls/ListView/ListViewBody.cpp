
#include "ListView.h"
#include "ListViewItem.h"
#include "ListViewHeader.h"
#include "ListViewBody.h"

ListViewBody::ListViewBody() : View("listviewbody-view")
{
	auto layout = createGridLayout();
	layout->setGap(0.0, 10.0);
}

void ListViewBody::updateColumns(ListViewHeader* header)
{
	std::vector<GridTrackSize> sizes;
	for (size_t i = 0, count = header->getColumnCount(); i < count; i++)
	{
		if (header->isColumnExpanding(i))
		{
			sizes.push_back(GridLayout::minmaxSize(header->getColumnWidth(i), GridLayout::autoSize));
		}
		else
		{
			sizes.push_back(header->getColumnWidth(i));
		}
	}
	getLayout<GridLayout>()->setColumns(sizes);
}

void ListViewBody::addViewBefore(ListViewItemView* view, ListViewItemView* sibling, ListViewHeader* header)
{
	getLayout<GridLayout>()->addViewBefore(view, sibling, 1, 0, header->getColumnCount());
}
