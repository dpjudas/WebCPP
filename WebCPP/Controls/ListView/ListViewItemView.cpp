
#include "ListViewItemView.h"
#include "ListViewItem.h"
#include "ListView.h"

ListViewItemView::ListViewItemView(ListViewItem* item) : View("listviewitem-view"), item(item)
{
	auto layout = createGridLayout();
	layout->setSubgrid(true, false);
}

View* ListViewItemView::getColumnView(size_t index)
{
	if (index < columns.size())
		return columns[index];
	else
		return nullptr;
}

void ListViewItemView::setColumnView(size_t index, View* view)
{
	if (index >= columns.size())
		columns.resize(index + 1);

	if (columns[index] != view)
	{
		delete columns[index];
		columns[index] = view;

		view->setParent(this);
		getLayout<GridLayout>()->addViewBefore(view, index + 1 < columns.size() ? columns[index + 1] : nullptr);

		item->listview()->onColumnViewChanged(this, index);
	}
}
