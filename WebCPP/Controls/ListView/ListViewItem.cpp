
#include "ListViewItem.h"
#include "ListView.h"
#include <stdexcept>
#include <algorithm>

ListViewItem::ListViewItem()
{
}

ListViewItem::~ListViewItem()
{
	removeAllChildren();
}

ListViewItem* ListViewItem::add(std::unique_ptr<ListViewItem> item)
{
	item->parentObj = this;
	item->prevSiblingObj = lastChildObj;
	if (lastChildObj)
		lastChildObj->nextSiblingObj = item.get();
	lastChildObj = item.get();
	if (!firstChildObj)
		firstChildObj = lastChildObj;

	ListView* lv = listview();
	if (lv)
		lv->onItemAttached(item.get());

	return item.release();
}

ListViewItem* ListViewItem::insertBefore(std::unique_ptr<ListViewItem> item, ListViewItem* sibling)
{
	if (!sibling)
		return add(std::move(item));
	if (sibling->parentObj != this)
		throw std::runtime_error("Sibling is not a child of this item");

	item->parentObj = this;
	if (!sibling->prevSiblingObj)
		firstChildObj = item.get();
	item->prevSiblingObj = sibling->prevSiblingObj;
	item->nextSiblingObj = sibling;
	sibling->prevSiblingObj = item.get();

	ListView* lv = listview();
	if (lv)
		lv->onItemAttached(item.get());

	return item.release();
}

std::unique_ptr<ListViewItem> ListViewItem::remove()
{
	removeAllChildren();

	ListView* lv = listview();
	std::unique_ptr<ListViewItem> item(this);

	if (prevSiblingObj)
		prevSiblingObj->nextSiblingObj = nextSiblingObj;
	if (nextSiblingObj)
		nextSiblingObj->prevSiblingObj = prevSiblingObj;
	if (parentObj)
	{
		if (parentObj->firstChildObj == this)
			parentObj->firstChildObj = nextSiblingObj;
		if (parentObj->lastChildObj == this)
			parentObj->lastChildObj = prevSiblingObj;
	}
	prevSiblingObj = nullptr;
	nextSiblingObj = nullptr;
	parentObj = nullptr;

	if (lv)
		lv->onItemDetached(this);

	return item;
}

void ListViewItem::removeAllChildren()
{
	while (lastChild())
		lastChild()->remove();
}

const ListViewItem* ListViewItem::root() const
{
	const ListViewItem* p = parentObj;
	if (p)
	{
		while (p->parentObj)
			p = p->parentObj;
	}
	else
	{
		p = this;
	}
	return p;
}

void ListViewItem::open()
{
	if (!openFlag)
	{
		ListView* lv = listview();
		if (lv)
			lv->openItem(this);
	}
}

void ListViewItem::close()
{
	if (openFlag)
	{
		ListView* lv = listview();
		if (lv)
			lv->closeItem(this);
	}
}

void ListViewItem::updateColumn(size_t index)
{
	if (view)
	{
		updateColumnView(index);
	}
}

void ListViewItem::sort()
{
	struct SortItem
	{
		SortItem(ListViewItem* lvitem, std::string sortCompareString) : lvitem(lvitem), sortCompareString(std::move(sortCompareString)) { }

		ListViewItem* lvitem;
		JSValue sortCompareString;
	};

	std::vector<SortItem> items;
	for (ListViewItem* cur = firstChild(); cur != nullptr; cur = cur->nextSibling())
		items.push_back({ cur, cur->sortCompareString() });

	std::stable_sort(items.begin(), items.end(), [](const SortItem& a, const SortItem& b) -> bool { return a.sortCompareString.call<int>("localeCompare", b.sortCompareString) < 0; });

	for (const SortItem& item : items)
	{
		auto child = item.lvitem->remove();
		add(std::move(child));
	}
}
