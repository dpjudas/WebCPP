#pragma once

#include "../../Core/View.h"
#include "../../Controls/TextLabel/TextLabel.h"
#include "../../Controls/ImageBox/ImageBox.h"
#include "ListViewItemView.h"
#include <list>
#include <memory>

namespace web
{
	class View;
	class ListView;
	class ListViewItemView;

	class ListViewItem
	{
	public:
		ListViewItem();
		virtual ~ListViewItem();

		ListViewItem* add(std::unique_ptr<ListViewItem> item);
		ListViewItem* insertBefore(std::unique_ptr<ListViewItem> item, ListViewItem* sibling);
		std::unique_ptr<ListViewItem> remove();
		void removeAllChildren();

		void open();
		void close();
		bool isOpen() const { return openFlag; }

		const std::string& id() const { return itemId; }
		void setId(std::string id) { itemId = std::move(id); }

		const ListViewItem* root() const;
		ListViewItem* parent() const { return parentObj; }
		ListViewItem* prevSibling() const { return prevSiblingObj; }
		ListViewItem* nextSibling() const { return nextSiblingObj; }
		ListViewItem* firstChild() const { return firstChildObj; }
		ListViewItem* lastChild() const { return lastChildObj; }

		ListViewItem* prevOpenItem() const;
		ListViewItem* nextOpenItem() const;

		ListViewItemView* itemview() const { return view; }

		virtual ListView* listview() const { const ListViewItem* r = root(); return (r != this) ? r->listview() : nullptr; }

		void updateColumn(size_t index);

		void sort();

	protected:
		virtual std::string sortCompareString() { return id(); }
		virtual void updateColumnView(size_t index) = 0;

	private:
		ListViewItem* parentObj = nullptr;
		ListViewItem* prevSiblingObj = nullptr;
		ListViewItem* nextSiblingObj = nullptr;
		ListViewItem* firstChildObj = nullptr;
		ListViewItem* lastChildObj = nullptr;

		std::string itemId;
		ListViewItemView* view = nullptr;
		bool openFlag = true;

		friend class ListView;
	};
}
