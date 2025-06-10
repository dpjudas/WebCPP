#pragma once

#include "ListViewItem.h"

class RootListViewItem : public ListViewItem
{
public:
	RootListViewItem(ListView* listview) : listviewObj(listview) {}

	ListView* listview() const override { return listviewObj; }

protected:
	std::string sortCompareString() override { return {}; }
	void updateColumnView(size_t index) override {}

private:
	ListView* listviewObj = nullptr;
};
