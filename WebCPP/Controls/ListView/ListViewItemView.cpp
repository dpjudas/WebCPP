
#include "WebCPP/Controls/ListView/ListViewItemView.h"
#include "WebCPP/Controls/ListView/ListViewItem.h"
#include "WebCPP/Controls/ListView/ListView.h"

namespace web
{
	ListViewItemView::ListViewItemView(ListViewItem* item) : View("listviewitem-view"), item(item)
	{
		auto layout = createGridLayout();
		layout->setSubgrid(true, false);
	}

	std::shared_ptr<View> ListViewItemView::getColumnView(size_t index)
	{
		if (index < columns.size())
			return columns[index];
		else
			return nullptr;
	}

	void ListViewItemView::setColumnView(size_t index, std::shared_ptr<View> view)
	{
		if (index >= columns.size())
			columns.resize(index + 1);

		if (columns[index] != view)
		{
			if (columns[index])
				columns[index]->detach();
			columns[index] = view;

			getLayout<GridLayout>()->addViewBefore(view, index + 1 < columns.size() ? columns[index + 1] : nullptr);

			item->listview()->onColumnViewChanged(this, index);
		}
	}
}
