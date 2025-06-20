#pragma once

#include "../../Core/View.h"

namespace web
{
	class ListViewItem;

	class ListViewItemView : public View
	{
	public:
		ListViewItemView(ListViewItem* item);

		View* getColumnView(size_t index);
		void setColumnView(size_t index, View* view);

		ListViewItem* getItem() const { return item; }

	private:
		ListViewItem* item = nullptr;
		std::vector<View*> columns;
	};
}
