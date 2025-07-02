#pragma once

#include "../../Core/View.h"

namespace web
{
	class ListViewItem;

	class ListViewItemView : public View
	{
	public:
		ListViewItemView(ListViewItem* item);

		std::shared_ptr<View> getColumnView(size_t index);
		void setColumnView(size_t index, std::shared_ptr<View> view);

		ListViewItem* getItem() const { return item; }

	private:
		ListViewItem* item = nullptr;
		std::vector<std::shared_ptr<View>> columns;
	};
}
