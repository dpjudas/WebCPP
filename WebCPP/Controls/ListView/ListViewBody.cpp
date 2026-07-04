
#include "WebCPP/Controls/ListView/ListView.h"
#include "WebCPP/Controls/ListView/ListViewItem.h"
#include "WebCPP/Controls/ListView/ListViewHeader.h"
#include "WebCPP/Controls/ListView/ListViewBody.h"

namespace web
{
	ListViewBody::ListViewBody() : View("listviewbody-view")
	{
		createGridLayout();
	}

	void ListViewBody::updateColumns(ListViewHeader* header)
	{
		size_t count = header->getColumnCount();

		auto isVisible = [&](size_t i) { return header->getColumnWidth(i) > 0.0 || header->isColumnExpanding(i); };

		std::vector<GridTrackSize> sizes;
		for (size_t i = 0; i < count; i++)
		{
			double width = header->getColumnWidth(i);

			// The header only renders a (10px) splitter after column i when column i itself is
			// visible and some later column is also visible. Mirror that exactly here, otherwise
			// a hidden column followed by a later visible column gets a phantom gap the header
			// doesn't have, desyncing the row grid from the header from that point onward.
			double gap = 0.0;
			if (isVisible(i))
			{
				for (size_t j = i + 1; j < count; j++)
				{
					if (isVisible(j))
					{
						gap = 10.0;
						break;
					}
				}
			}

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
