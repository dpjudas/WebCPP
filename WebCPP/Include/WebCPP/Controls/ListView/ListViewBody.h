#pragma once

#include "../../Core/View.h"

namespace web
{
	class ListViewHeader;
	class ListViewItemView;

	class ListViewBody : public View
	{
	public:
		ListViewBody();

		void updateColumns(ListViewHeader* header);
		void addViewBefore(std::shared_ptr<ListViewItemView> view, std::shared_ptr<ListViewItemView> sibling, ListViewHeader* header);
	};
}
