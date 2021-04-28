#pragma once

#include "WebCPP/Core/View.h"
#include "ListViewItem.h"
#include <list>

class TextLabel;
class ListViewHeader;
class ListViewBody;

class ListView : public VBoxView
{
public:
	ListView(View* parent);
	~ListView();

	void addColumn(std::string name, double width, bool expanding = false);
	ListViewItem* rootItem() { return root.get(); }
	std::vector<ListViewItem*> selectedItems();
	ListViewItem* selectedItem();

	void setAlternatingRowColors(bool enable);

	void selectItem(ListViewItem* item);

	std::function<void(ListViewItem*)> activated;
	std::function<void(ListViewItem*)> clicked;
	std::function<void(ListViewItem*)> doubleClicked;
	std::function<void(ListViewItem*)> collapsed;
	std::function<void(ListViewItem*)> expanded;
	std::function<void()> selectionChanged;

private:
	void onBodyClick(Event* event);
	void onItemClick(Event* event, ListViewItem* item);
	void onItemAttached(ListViewItem* item);
	void onItemDetached(ListViewItem* item);
	void openItem(ListViewItem* item);
	void closeItem(ListViewItem* item);
	void createItemView(ListViewItem* item);
	void onColumnViewChanged(ListViewItemView* itemview, size_t index);

	ListViewHeader* header = nullptr;
	ListViewBody* body = nullptr;
	std::unique_ptr<ListViewItem> root;
	ListViewItem* curSelectedItem = nullptr;

	friend class ListViewItem;
	friend class ListViewItemView;
};

class ListViewHeader : public HBoxView
{
public:
	ListViewHeader(View* parent) : HBoxView(parent)
	{
		addClass("listviewheader");
	}

	struct Column
	{
		TextLabel* label = nullptr;
		double width = 0.0;
		bool expanding = false;
		View* splitter = nullptr;
	};

	std::vector<Column> columns;
};

class ListViewBody : public VBoxView
{
public:
	ListViewBody(View* parent) : VBoxView(parent)
	{
		addClass("listviewbody");
	}
};
