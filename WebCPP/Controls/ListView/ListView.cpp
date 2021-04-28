
#include "ListView.h"
#include "ListViewItem.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"

ListView::ListView(View* parent) : VBoxView(parent)
{
	addClass("listview");
	header = new ListViewHeader(this);
	header->addClass("listview-header");
	body = new ListViewBody(this);
	body->setExpanding();
	header->addClass("listview-body");
	root.reset(new RootListViewItem(this));
	body->element->addEventListener("click", [=](Event* e) { onBodyClick(e); });
}

ListView::~ListView()
{
	root.reset();
}

void ListView::onBodyClick(Event* event)
{
	event->stopPropagation();
	event->preventDefault();
	selectItem(nullptr);
}

void ListView::onItemClick(Event* event, ListViewItem* item)
{
	int detail = event->handle["detail"].as<int>();
	event->stopPropagation();

	selectItem(item);

	if (detail == 1)
	{
		if (clicked)
			clicked(item);
	}
	else if (detail == 2)
	{
		if (doubleClicked)
			doubleClicked(item);

		if (activated)
			activated(item);
	}
}

void ListView::setAlternatingRowColors(bool enable)
{
	if (enable)
		body->addClass("alternate");
	else
		body->removeClass("alternate");
}

void ListView::selectItem(ListViewItem* item)
{
	if (curSelectedItem != item)
	{
		if (curSelectedItem && curSelectedItem->view)
			curSelectedItem->view->removeClass("selected");
		curSelectedItem = item;
		if (curSelectedItem && curSelectedItem->view)
			curSelectedItem->view->addClass("selected");
		if (selectionChanged)
			selectionChanged();
	}
}

void ListView::addColumn(std::string name, double width, bool expanding)
{
	ListViewHeader::Column col;
	if (!header->columns.empty())
	{
		col.splitter = new HBoxView(header);
		col.splitter->addClass("listview-headersplitter");
		col.splitter->element->setStyle("width", "10px");
		new View(col.splitter);
	}
	col.label = new TextLabel(header);
	col.label->addClass("listview-headerlabel");
	col.label->setText(name);
	col.label->element->setStyle("width", std::to_string(width) + "px");
	if (expanding)
		col.label->element->setStyle("flexGrow", "1");
	col.width = width;
	col.expanding = expanding;
	header->columns.push_back(col);
}

std::vector<ListViewItem*> ListView::selectedItems()
{
	std::vector<ListViewItem*> result;
	if (curSelectedItem)
		result.push_back(curSelectedItem);
	return result;
}

ListViewItem* ListView::selectedItem()
{
	auto items = selectedItems();
	return (items.size() == 1) ? items.front() : nullptr;
}

void ListView::openItem(ListViewItem* item)
{
	if (item != root.get())
	{
		item->openFlag = true;
		onItemAttached(item);
	}
}

void ListView::closeItem(ListViewItem* item)
{
	if (item != root.get())
	{
		onItemDetached(item);
		item->openFlag = false;
	}
}

void ListView::onItemAttached(ListViewItem* item)
{
	if (item != root.get())
	{
		createItemView(item);

		if (item->isOpen())
		{
			for (ListViewItem* cur = item->firstChild(); cur != nullptr; cur = cur->nextSibling())
			{
				onItemAttached(cur);
			}
		}
	}
}

void ListView::onItemDetached(ListViewItem* item)
{
	if (item != root.get() && item->view)
	{
		if (item->isOpen())
		{
			for (ListViewItem* cur = item->firstChild(); cur != nullptr; cur = cur->nextSibling())
			{
				onItemDetached(cur);
			}
		}

		delete item->view;
		item->view = nullptr;
	}
}

void ListView::createItemView(ListViewItem* item)
{
	item->view = new ListViewItemView(item, body);
	item->view->element->addEventListener("click", [=](Event* e) { onItemClick(e, item); });

	ListViewItem* nextItem = item;
	while (nextItem && !nextItem->nextSibling())
		nextItem = nextItem->parent();
	if (nextItem)
		nextItem = nextItem->nextSibling();
	if (nextItem && nextItem->view)
		item->view->moveBefore(nextItem->view);

	for (size_t i = 0; i < header->columns.size(); i++)
	{
		item->updateColumn(i);

		View* columnView = item->view->getColumnView(i);
		if (!columnView)
		{
			columnView = new View(item->view);
			item->view->setColumnView(i, columnView);
		}
	}
}

void ListView::onColumnViewChanged(ListViewItemView *itemview, size_t i)
{
	double splitterSize = (i + 1 < header->columns.size()) ? 10 : 0;

	View* columnView = itemview->getColumnView(i);

	if (i == 0)
	{
		int depth = 0;
		ListViewItem* p = itemview->getItem()->parent();
		while (p && p != root.get())
		{
			depth++;
			p = p->parent();
		}

		if (i == 0 && depth > 0)
		{
			double padding = depth * 24;
			double width = std::max(header->columns[i].width - padding, 0.0);
			columnView->element->setStyle("width", std::to_string(width + splitterSize) + "px");
			columnView->element->setStyle("padding-left", std::to_string(padding) + "px");
		}
		else
		{
			columnView->element->setStyle("width", std::to_string(header->columns[i].width + splitterSize) + "px");
		}
	}
	else
	{
		columnView->element->setStyle("width", std::to_string(header->columns[i].width + splitterSize) + "px");
	}

	if (header->columns[i].expanding)
		columnView->element->setStyle("flexGrow", "1");
}
