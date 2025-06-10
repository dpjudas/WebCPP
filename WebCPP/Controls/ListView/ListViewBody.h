#pragma once

#include "WebCPP/Core/View.h"

class ListViewHeader;
class ListViewItemView;

class ListViewBody : public View
{
public:
	ListViewBody();

	void updateColumns(ListViewHeader* header);
	void addViewBefore(ListViewItemView* view, ListViewItemView* sibling, ListViewHeader* header);
};
