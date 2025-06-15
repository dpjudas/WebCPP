
#include "WebCPP/Controls/ListView/ListView.h"
#include "WebCPP/Controls/ListView/ListViewItem.h"
#include "WebCPP/Controls/ListView/ListViewHeader.h"
#include "WebCPP/Controls/ListView/ListViewBody.h"
#include "WebCPP/Controls/ListView/RootListViewItem.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"
#include "WebCPP/Controls/Scrollbar/Scrollbar.h"
#include "WebCPP/Controls/Menu/Menu.h"

namespace web
{
	ListView::ListView() : View("listview-view")
	{
		setupUi();
		root.reset(new RootListViewItem(this));
	}

	ListView::~ListView()
	{
		root.reset();
	}

	void ListView::setupUi()
	{
		body = new ListViewBody();
		body->element->setTabIndex(0);
		header = new ListViewHeader();
		header->addClass("listview-header");
		header->addClass("listview-body");
		scrollVert = new Scrollbar(ScrollbarDirection::vertical);
		scrollVert->addClass("listview-scrollvert");
		scrollHorz = new Scrollbar(ScrollbarDirection::horizontal);
		scrollHorz->addClass("listview-scrollhorz");
		scrollCorner = new ScrollbarCorner();
		scrollCorner->addClass("listview-scrollcorner");
		header->columnsUpdated = [=]() { body->updateColumns(header); updateScrollbars(); };
		body->element->addEventListener("click", [=](Event* e) { onBodyClick(e); });
		body->element->addEventListener("scroll", [=](Event* e) { onBodyScroll(e); });
		body->element->addEventListener("focus", [=](Event* e) { onBodyFocus(e); });
		body->element->addEventListener("blur", [=](Event* e) { onBodyBlur(e); });
		body->element->addEventListener("keydown", [=](Event* e) { onBodyKeyDown(e); });
		scrollVert->scroll = [=]() { onScrollbarScroll(); };
		scrollHorz->scroll = [=]() { onScrollbarScroll(); };

		auto layout = createGridLayout();
		layout->setColumns({ GridLayout::autoSize, GridLayout::minContentSize });
		layout->setRows({ GridLayout::autoSize, GridLayout::autoSize, GridLayout::freeSpace, GridLayout::minContentSize });
		layout->addView(header, 1, 1);
		layout->addView(body, 1, 2);
		layout->addView(scrollVert, 2, 1, 1, 3);
		layout->addView(scrollHorz, 1, 4);
		layout->addView(scrollCorner, 2, 4);

		body->addClass("uses-scrollbar");

		resizeObserver.onResize = [=](std::vector<ResizeObserverEntry> entries) { onResize(std::move(entries)); };
		resizeObserver.observe(body->element.get());
	}

	void ListView::onResize(std::vector<ResizeObserverEntry> entries)
	{
		updateScrollbars();
	}

	void ListView::onBodyFocus(Event* event)
	{
		addClass("focused");
		if (!focusedItem())
			focusItem(selectedItem());
	}

	void ListView::onBodyBlur(Event* event)
	{
		removeClass("focused");
	}

	void ListView::onBodyKeyDown(Event* event)
	{
		int keyCode = event->handle["keyCode"].as<int>();
		bool processed = true;
		if (keyCode == 13) // Enter
		{
			if (activated && focusedItem())
				activated(focusedItem());
		}
		else if (keyCode == 33) // Page up
		{
			ListViewItem* item = focusedItem();
			if (item && item->view)
			{
				double pageHeight = body->element->clientHeight();
				double offsetHeight = item->view->element->offsetHeight();
				if (offsetHeight >= 1)
				{
					int itemsPerPage = (int)(pageHeight / offsetHeight);
					for (int i = 0; i < itemsPerPage; i++)
					{
						ListViewItem* prev = item->prevOpenItem();
						if (prev)
							item = prev;
						else
							break;
					}

					focusItem(item);
					selectItem(item);
				}
			}
		}
		else if (keyCode == 34) // Page down
		{
			ListViewItem* item = focusedItem();
			if (item && item->view)
			{
				double pageHeight = body->element->clientHeight();
				double offsetHeight = item->view->element->offsetHeight();
				if (offsetHeight >= 1)
				{
					int itemsPerPage = (int)(pageHeight / offsetHeight);
					for (int i = 0; i < itemsPerPage; i++)
					{
						ListViewItem* next = item->nextOpenItem();
						if (next)
							item = next;
						else
							break;
					}

					focusItem(item);
					selectItem(item);
				}
			}
		}
		else if (keyCode == 35) // End
		{
			ListViewItem* item = rootItem()->lastChild();
			while (item && item->lastChild())
				item = item->lastChild();
			if (item)
			{
				focusItem(item);
				selectItem(item);
			}
		}
		else if (keyCode == 36) // Home
		{
			ListViewItem* item = rootItem()->firstChild();
			if (item)
			{
				focusItem(item);
				selectItem(item);
			}
		}
		else if (keyCode == 38) // Arrow up
		{
			ListViewItem* item = focusedItem() ? focusedItem()->prevOpenItem() : nullptr;
			if (item)
			{
				focusItem(item);
				selectItem(item);
			}
		}
		else if (keyCode == 40) // Arrow down
		{
			ListViewItem* item = focusedItem() ? focusedItem()->nextOpenItem() : nullptr;
			if (item)
			{
				focusItem(item);
				selectItem(item);
			}
		}
		else
		{
			// printf("keycode: %d\n", keyCode);
			processed = false;
		}

		if (processed)
		{
			event->preventDefault();
			event->stopPropagation();
		}
	}

	void ListView::onBodyClick(Event* event)
	{
		event->stopPropagation();
		event->preventDefault();
		selectItem(nullptr);
	}

	void ListView::updateScrollbars()
	{
		double scrollLeft = body->element->scrollLeft();
		double scrollTop = body->element->scrollTop();
		double scrollWidth = body->element->scrollWidth();// std::max(body->element->scrollWidth(), header->element->scrollWidth());
		double scrollHeight = body->element->scrollHeight();
		double pageWidth = body->element->clientWidth();
		double pageHeight = body->element->clientHeight();
		scrollHorz->setPosition(scrollLeft / scrollWidth, (scrollLeft + pageWidth) / scrollWidth);
		scrollVert->setPosition(scrollTop / scrollHeight, (scrollTop + pageHeight) / scrollHeight);

		bool horzScrollVisible = (scrollWidth > pageWidth + 1.0);
		scrollHorz->setVisible(horzScrollVisible);

		bool vertScrollVisible = (scrollHeight > pageHeight + 1.0);
		scrollVert->setVisible(vertScrollVisible);

		scrollCorner->setVisible(horzScrollVisible && vertScrollVisible);
	}

	void ListView::onScrollbarScroll()
	{
		double scrollWidth = body->element->scrollWidth();// std::max(body->element->scrollWidth(), header->element->scrollWidth());
		double scrollHeight = body->element->scrollHeight();
		header->element->scrollTo(scrollHorz->getPosition() * scrollWidth, 0.0);
		body->element->scrollTo(scrollHorz->getPosition() * scrollWidth, scrollVert->getPosition() * scrollHeight);
	}

	void ListView::onBodyScroll(Event* event)
	{
		updateScrollbars();
		if (scroll)
			scroll();
	}

	void ListView::onItemClick(Event* event, ListViewItem* item)
	{
		int detail = event->handle["detail"].as<int>();
		event->stopPropagation();

		body->element->focus();
		addClass("focused");
		focusItem(item);
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

	void ListView::onItemContextMenu(Event* event, ListViewItem* item)
	{
		event->stopPropagation();
		event->preventDefault();
		focusItem(item);
		selectItem(item);
		if (onContextMenu)
		{
			double clientX = event->clientX();
			double clientY = event->clientY();
			auto openMenu = new Menu();
			onContextMenu(item, openMenu);
			openMenu->showModal();
			openMenu->setLeftPosition(clientX, clientY);
			openMenu->parent()->element->addEventListener("click", [=](Event* event) { event->stopPropagation(); openMenu->closeModal(); });
			openMenu->closeMenu = [=]() { openMenu->closeModal(); };
		}
	}

	void ListView::setAlternatingRowColors(bool enable)
	{
		if (enable)
			body->addClass("alternate");
		else
			body->removeClass("alternate");
	}

	void ListView::scrollToItem(ListViewItem* item, ScrollToHint hint)
	{
		if (item->view)
		{
			double headerHeight = header->element->clientHeight();
			double pageHeight = body->element->clientHeight();
			double offsetTop = item->view->element->offsetTop();
			double offsetHeight = item->view->element->offsetHeight();
			if (hint == ScrollToHint::ensureVisible)
			{
				double scrollTop = body->element->scrollTop();
				if (offsetTop < scrollTop + headerHeight)
					body->element->setScrollTop(std::max(offsetTop - headerHeight, 0.0));
				else if (offsetTop + offsetHeight > scrollTop + pageHeight)
					body->element->setScrollTop(std::max(offsetTop + offsetHeight - pageHeight, 0.0));
			}
			else if (hint == ScrollToHint::positionAtTop)
			{
				body->element->setScrollTop(std::max(offsetTop, headerHeight));
			}
			else if (hint == ScrollToHint::positionAtBottom)
			{
				body->element->setScrollTop(std::max(offsetTop + offsetHeight - pageHeight, 0.0));
			}
			else if (hint == ScrollToHint::positionAtCenter)
			{
				body->element->setScrollTop(std::max(offsetTop + (offsetHeight - pageHeight) / 2.0 - headerHeight, 0.0));
			}
		}
	}

	ListViewItem* ListView::findItem(const std::string& id, const bool recursive, ListViewItem* item) const
	{
		ListViewItem* child = item != nullptr ? item : rootItem()->firstChild();
		while (child != nullptr)
		{
			if (child->id() == id)
			{
				return child;
			}
			else if (recursive && child->firstChild() != nullptr)
			{
				if (auto foundItem = findItem(id, recursive, child->firstChild()))
					return foundItem;
			}
			child = child->nextSibling();
		}
		return nullptr;
	}

	double ListView::scrollTop() const
	{
		return body->element->scrollTop();
	}

	double ListView::scrollPage() const
	{
		return body->element->clientHeight();
	}

	double ListView::scrollMax() const
	{
		return body->element->scrollHeight();
	}

	void ListView::setScrollTop(double y)
	{
		body->element->setScrollTop(y);
	}

	void ListView::focusItem(ListViewItem* item)
	{
		if (curFocusItem != item)
		{
			if (curFocusItem && curFocusItem->view)
				curFocusItem->view->removeClass("focused");
			curFocusItem = item;
			if (curFocusItem && curFocusItem->view)
				curFocusItem->view->addClass("focused");
			if (curFocusItem)
				scrollToItem(curFocusItem);
		}
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

	void ListView::clearList()
	{
		if (root != nullptr)
			root->removeAllChildren();
	}

	int ListView::addColumn(std::string name, double width, bool expanding)
	{
		header->addColumn(name, width, expanding);
		body->updateColumns(header);
		return header->getColumnCount() - 1;
	}

	std::vector<std::string> ListView::columnNames() const
	{
		return header->columnNames();
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
				for (ListViewItem* cur = item->lastChild(); cur != nullptr; cur = cur->prevSibling())
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
		item->view = new ListViewItemView(item);

		item->view->element->addEventListener("click", [=](Event* e) { onItemClick(e, item); });
		item->view->element->addEventListener("contextmenu", [=](Event* e) { onItemContextMenu(e, item); });

		ListViewItem* nextItem = item;
		while (nextItem && !nextItem->nextSibling())
			nextItem = nextItem->parent();
		if (nextItem)
			nextItem = nextItem->nextSibling();

		body->addViewBefore(item->view, nextItem && nextItem->view ? nextItem->view : nullptr, header);

		for (size_t i = 0, count = header->getColumnCount(); i < count; i++)
		{
			item->updateColumn(i);

			View* columnView = item->view->getColumnView(i);
			if (!columnView)
			{
				columnView = new View();
				item->view->setColumnView(i, columnView);
			}
		}
	}

	void ListView::onColumnViewChanged(ListViewItemView* itemview, size_t i)
	{
		double splitterSize = (i + 1 < header->getColumnCount()) ? 10 : 0;

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
				columnView->element->setStyle("padding-left", std::to_string(padding) + "px");
			}
		}
		else
		{
			// columnView->element->setStyle("padding-left", "5px");
		}
	}

	bool ListView::setFocus()
	{
		body->element->focus();
		return true;
	}
}
