#pragma once

#include "WebCPP/Core/View.h"
#include "WebCPP/Core/ResizeObserver.h"
#include "ListViewItem.h"
#include "TextListViewItem.h"
#include <list>

class TextLabel;
class ListViewHeader;
class ListViewHeaderSplitter;
class ListViewBody;
class Menu;
class Scrollbar;
class ScrollbarCorner;

enum class ScrollToHint
{
	ensureVisible,
	positionAtTop,
	positionAtBottom,
	positionAtCenter
};

class ListView : public View
{
public:
	ListView();
	~ListView();

	void clearList();

	int addColumn(std::string name, double width, bool expanding = false);

	std::vector<std::string> columnNames() const;

	ListViewItem* rootItem() { return root.get(); }
	const ListViewItem* rootItem() const { return root.get(); }
	std::vector<ListViewItem*> selectedItems();
	ListViewItem* selectedItem();
	ListViewItem* focusedItem() { return curFocusItem; }

	void setAlternatingRowColors(bool enable);

	void scrollToItem(ListViewItem* item, ScrollToHint hint = ScrollToHint::ensureVisible);

	ListViewItem* findItem(const std::string& id, const bool recursive = false, ListViewItem* item = nullptr) const;

	double scrollTop() const;
	double scrollPage() const;
	double scrollMax() const;
	void setScrollTop(double y);

	void selectItem(ListViewItem* item);
	void focusItem(ListViewItem* item);

	std::function<void(ListViewItem*)> activated;
	std::function<void(ListViewItem*)> clicked;
	std::function<void(ListViewItem*)> doubleClicked;
	std::function<void(ListViewItem*, Menu*)> onContextMenu;
	std::function<void(ListViewItem*)> collapsed;
	std::function<void(ListViewItem*)> expanded;
	std::function<void()> selectionChanged;
	std::function<void()> scroll;

	bool setFocus() override;

private:
	void setupUi();
	void onBodyClick(Event* event);
	void onBodyScroll(Event* event);
	void onBodyFocus(Event* event);
	void onBodyBlur(Event* event);
	void onBodyKeyDown(Event* event);
	void onItemClick(Event* event, ListViewItem* item);
	void onItemContextMenu(Event* event, ListViewItem* item);
	void onItemAttached(ListViewItem* item);
	void onItemDetached(ListViewItem* item);
	void openItem(ListViewItem* item);
	void closeItem(ListViewItem* item);
	void createItemView(ListViewItem* item);
	void onColumnViewChanged(ListViewItemView* itemview, size_t index);
	void updateScrollbars();
	void onScrollbarScroll();
	void onResize(std::vector<ResizeObserverEntry> entries);

	ListViewHeader* header = nullptr;
	ListViewBody* body = nullptr;
	std::unique_ptr<ListViewItem> root;
	ListViewItem* curSelectedItem = nullptr;
	ListViewItem* curFocusItem = nullptr;
	Scrollbar* scrollVert = nullptr;
	Scrollbar* scrollHorz = nullptr;
	ScrollbarCorner* scrollCorner = nullptr;
	ResizeObserver resizeObserver;

	friend class ListViewItem;
	friend class ListViewItemView;
};
