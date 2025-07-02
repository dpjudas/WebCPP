
#include "WebCPP/Core/ViewLayout.h"
#include "WebCPP/Core/View.h"
#include "WebCPP/Core/Element.h"
#include "WebCPP/Core/ShadowRoot.h"

namespace web
{
	ViewLayout::ViewLayout(View* owner) : owner(owner)
	{
	}

	ViewLayout::~ViewLayout()
	{
		clear();
	}

	void ViewLayout::clear()
	{
		while (!items.empty())
			detachItem(items.back().get());
	}

	void ViewLayout::detachItem(ViewLayoutItem* item)
	{
		item->view->layoutItem = nullptr;
		removeChild(item->view->element.get());
		items.erase(item->it);
	}

	bool ViewLayout::forceFocus()
	{
		for (auto& item : items)
		{
			if (item->view->getVisible())
			{
				if (item->view->defaultFocused)
				{
					item->view->setFocus();
					return true;
				}
				else if (item->view->forceFocus())
				{
					return true;
				}
			}
		}
		return false;
	}

	bool ViewLayout::focusFirstItem()
	{
		for (auto& item : items)
		{
			if (item->view->getVisible())
			{
				if (item->view->setFocus())
				{
					item->view->setFocus();
					return true;
				}
				else if (item->view->focusFirstChild())
				{
					return true;
				}
			}
		}
		return false;
	}

	void ViewLayout::addStickyView(std::shared_ptr<View> view)
	{
		attachView(view);
		view->element->setStyle("position", "sticky");
		appendChild(view->element.get());
	}

	void ViewLayout::addAbsoluteView(std::shared_ptr<View> view)
	{
		attachView(view);
		view->element->setStyle("position", "absolute");
		appendChild(view->element.get());
	}

	void ViewLayout::addFixedView(std::shared_ptr<View> view)
	{
		attachView(view);
		view->element->setStyle("position", "fixed");
		appendChild(view->element.get());
	}

	void ViewLayout::appendChild(Element* element)
	{
		if (owner->shadowRoot)
			owner->shadowRoot->appendChild(element);
		else
			owner->element->appendChild(element);
	}

	void ViewLayout::insertBefore(Element* newElement, Element* insertPoint)
	{
		if (owner->shadowRoot)
			owner->shadowRoot->insertBefore(newElement, insertPoint);
		else
			owner->element->insertBefore(newElement, insertPoint);
	}

	void ViewLayout::removeChild(Element* element)
	{
		if (owner->shadowRoot)
			owner->shadowRoot->removeChild(element);
		else
			owner->element->removeChild(element);
	}

	void ViewLayout::attachView(std::shared_ptr<View> view)
	{
		items.push_back(std::make_unique<ViewLayoutItem>());
		items.back()->it = --items.end();
		items.back()->layout = this;
		items.back()->view = std::move(view);
	}

	//////////////////////////////////////////////////////////////////////////////

	FlowLayout::FlowLayout(View* owner) : ViewLayout(owner)
	{
	}

	FlowLayout::~FlowLayout()
	{
	}

	void FlowLayout::addView(std::shared_ptr<View> view)
	{
		attachView(view);
		appendChild(view->element.get());
	}

	//////////////////////////////////////////////////////////////////////////////

	FlexLayout::FlexLayout(View* owner) : ViewLayout(owner)
	{
		owner->element->setStyle("display", "flex");
	}

	FlexLayout::~FlexLayout()
	{
	}

	void FlexLayout::setDirection(FlexDirection direction)
	{
		std::string value;
		switch (direction)
		{
		case FlexDirection::row: value = "row"; break;
		case FlexDirection::rowReverse: value = "rowReverse"; break;
		case FlexDirection::column: value = "column"; break;
		case FlexDirection::columnReverse: value = "columnReverse"; break;
		}
		owner->element->setStyle("flex-direction", value);
	}

	void FlexLayout::setWrap(FlexWrap wrap)
	{
		std::string value;
		switch (wrap)
		{
		case FlexWrap::nowrap: value = "nowrap"; break;
		case FlexWrap::wrap: value = "wrap"; break;
		case FlexWrap::wrapReverse: value = "wrap-reverse"; break;
		}
		owner->element->setStyle("flex-wrap", value);
	}

	void FlexLayout::setJustifyContent(FlexJustifyContent justify)
	{
		std::string value;
		switch (justify)
		{
		case FlexJustifyContent::flexStart: value = "flex-start"; break;
		case FlexJustifyContent::flexEnd: value = "flex-end"; break;
		case FlexJustifyContent::center: value = "center"; break;
		case FlexJustifyContent::spaceBetween: value = "space-between"; break;
		case FlexJustifyContent::spaceAround: value = "space-around"; break;
		case FlexJustifyContent::spaceEvenly: value = "space-evenly"; break;
		}
		owner->element->setStyle("justify-content", value);
	}

	void FlexLayout::setAlignItems(FlexAlignItems align)
	{
		std::string value;
		switch (align)
		{
		case FlexAlignItems::flexStart: value = "flex-start"; break;
		case FlexAlignItems::flexEnd: value = "flex-end"; break;
		case FlexAlignItems::center: value = "center"; break;
		case FlexAlignItems::stretch: value = "stretch"; break;
		case FlexAlignItems::baseline: value = "baseline"; break;
		}
		owner->element->setStyle("align-items", value);
	}

	void FlexLayout::setGap(double length)
	{
		owner->element->setStyle("gap", std::to_string(length) + "px");
	}

	void FlexLayout::setGap(double rowLength, double columnLength)
	{
		owner->element->setStyle("gap", std::to_string(rowLength) + " " + std::to_string(columnLength) + "px");
	}

	void FlexLayout::addViewBefore(std::shared_ptr<View> view, std::shared_ptr<View> sibling, bool grow, bool shrink, FlexAlignSelf align, int order)
	{
		if (sibling && sibling->parent() != owner)
			sibling = nullptr;

		if (grow && shrink)
		{
			view->element->setStyle("flex", "1 1 auto");
			view->element->setStyle("min-width", "0");
			view->element->setStyle("min-height", "0");
		}
		else if (grow)
		{
			view->element->setStyle("flex", "1 0 auto");
		}
		else if (shrink)
		{
			view->element->setStyle("flex", "0 1 auto");
			view->element->setStyle("min-width", "0");
			view->element->setStyle("min-height", "0");
		}
		else
		{
			view->element->setStyle("flex", "0 0 auto");
		}

		std::string value;
		switch (align)
		{
		case FlexAlignSelf::useAlignItems: /* value = "auto"; */ break;
		case FlexAlignSelf::flexStart: value = "flex-start"; break;
		case FlexAlignSelf::flexEnd: value = "flex-end"; break;
		case FlexAlignSelf::center: value = "center"; break;
		case FlexAlignSelf::stretch: value = "stretch"; break;
		case FlexAlignSelf::baseline: value = "baseline"; break;
		}
		if (!value.empty())
			view->element->setStyle("align-self", value);

		if (order != 0)
		{
			view->element->setStyle("order", std::to_string(order));
		}

		attachView(view);

		if (!sibling)
			appendChild(view->element.get());
		else
			insertBefore(view->element.get(), sibling->element.get());
	}

	void FlexLayout::addSpacer()
	{
		auto element = std::make_unique<Element>("spacer-view");
		element->setStyle("flex", "1 1 0");
		appendChild(element.get());
		spacers.push_back(std::move(element));
	}

	//////////////////////////////////////////////////////////////////////////////

	const GridTrackSize GridLayout::autoSize("auto");
	const GridTrackSize GridLayout::minContentSize("min-content");
	const GridTrackSize GridLayout::maxContentSize("max-content");
	const GridTrackSize GridLayout::fitContentSize("fit-content()");
	const GridTrackSize GridLayout::minSize("min()");
	const GridTrackSize GridLayout::maxSize("max()");
	const GridTrackSize GridLayout::freeSpace("1fr");

	GridTrackSize GridLayout::minmaxSize(GridTrackSize minval, GridTrackSize maxval)
	{
		return GridTrackSize("minmax(" + minval.value + ", " + maxval.value + ")");
	}

	GridLayout::GridLayout(View* owner) : ViewLayout(owner)
	{
		owner->element->setStyle("display", "grid");
	}

	std::string GridLayout::toString(const std::vector<GridTrackSize>& sizes)
	{
		std::string value;
		size_t len = 0;
		for (const GridTrackSize& size : sizes)
		{
			if (len == 0)
				len++;
			len += size.value.size();
		}
		value.reserve(len);
		for (const GridTrackSize& size : sizes)
		{
			if (!value.empty())
				value.push_back(' ');
			value += size.value;
		}
		return value;
	}

	void GridLayout::setColumns(const std::vector<GridTrackSize>& sizes)
	{
		owner->element->setStyle("grid-template-columns", toString(sizes));
	}

	void GridLayout::setRows(const std::vector<GridTrackSize>& sizes)
	{
		owner->element->setStyle("grid-template-rows", toString(sizes));
	}

	void GridLayout::setAutoColumns(const std::vector<GridTrackSize>& sizes)
	{
		owner->element->setStyle("grid-auto-columns", toString(sizes));
	}

	void GridLayout::setAutoRows(const std::vector<GridTrackSize>& sizes)
	{
		owner->element->setStyle("grid-auto-rows", toString(sizes));
	}

	void GridLayout::setGap(double length)
	{
		owner->element->setStyle("gap", std::to_string(length) + "px");
	}

	void GridLayout::setGap(double rowLength, double columnLength)
	{
		owner->element->setStyle("gap", std::to_string(rowLength) + "px " + std::to_string(columnLength) + "px");
	}

	void GridLayout::setJustifyItems(GridJustifyItems justify)
	{
		std::string value;
		switch (justify)
		{
		case GridJustifyItems::start: value = "start"; break;
		case GridJustifyItems::end: value = "end"; break;
		case GridJustifyItems::center: value = "center"; break;
		case GridJustifyItems::stretch: value = "stretch"; break;
		}
		owner->element->setStyle("justify-items", value);
	}

	void GridLayout::setAlignItems(GridAlignItems align)
	{
		std::string value;
		switch (align)
		{
		case GridAlignItems::stretch: value = "stretch"; break;
		case GridAlignItems::start: value = "start"; break;
		case GridAlignItems::end: value = "end"; break;
		case GridAlignItems::center: value = "center"; break;
		case GridAlignItems::baseline: value = "baseline"; break;
		}
		owner->element->setStyle("align-items", value);
	}

	void GridLayout::setJustifyContent(GridJustifyContent justify)
	{
		std::string value;
		switch (justify)
		{
		case GridJustifyContent::start: value = "start"; break;
		case GridJustifyContent::end: value = "end"; break;
		case GridJustifyContent::center: value = "center"; break;
		case GridJustifyContent::stretch: value = "stretch"; break;
		case GridJustifyContent::spaceBetween: value = "space-between"; break;
		case GridJustifyContent::spaceAround: value = "space-around"; break;
		case GridJustifyContent::spaceEvenly: value = "space-evenly"; break;
		}
		owner->element->setStyle("justify-content", value);
	}

	void GridLayout::setAlignContent(GridAlignContent align)
	{
		std::string value;
		switch (align)
		{
		case GridAlignContent::start: value = "start"; break;
		case GridAlignContent::end: value = "end"; break;
		case GridAlignContent::center: value = "center"; break;
		case GridAlignContent::stretch: value = "stretch"; break;
		case GridAlignContent::spaceBetween: value = "space-between"; break;
		case GridAlignContent::spaceAround: value = "space-around"; break;
		case GridAlignContent::spaceEvenly: value = "space-evenly"; break;
		}
		owner->element->setStyle("align-content", value);
	}

	void GridLayout::setAutoFlow(GridAutoFlow flow)
	{
		std::string value;
		switch (flow)
		{
		case GridAutoFlow::row: value = "row"; break;
		case GridAutoFlow::column: value = "column"; break;
		case GridAutoFlow::rowDense: value = "row dense"; break;
		case GridAutoFlow::columnDense: value = "column dense"; break;
		}
		owner->element->setStyle("grid-auto-flow", value);
	}

	void GridLayout::setSubgrid(bool column, bool row)
	{
		if (column)
			owner->element->setStyle("grid-template-columns", "subgrid");
		if (row)
			owner->element->setStyle("grid-template-rows", "subgrid");
	}

	void GridLayout::addViewBefore(std::shared_ptr<View> view, std::shared_ptr<View> sibling, int column, int row, int colspan, int rowspan, GridJustifySelf justify, GridAlignSelf align)
	{
		if (sibling && sibling->parent() != owner)
			sibling = nullptr;

		if (column != 0)
		{
			if (colspan == 1)
				view->element->setStyle("grid-column", std::to_string(column));
			else
				view->element->setStyle("grid-column", std::to_string(column) + " / span " + std::to_string(colspan));
		}

		if (row != 0)
		{
			if (rowspan == 1)
				view->element->setStyle("grid-row", std::to_string(row));
			else
				view->element->setStyle("grid-row", std::to_string(row) + " / span " + std::to_string(rowspan));
		}

		if (justify != GridJustifySelf::useJustifyItems)
		{
			std::string value;
			switch (justify)
			{
			case GridJustifySelf::useJustifyItems: value = "auto"; break;
			case GridJustifySelf::start: value = "start"; break;
			case GridJustifySelf::end: value = "end"; break;
			case GridJustifySelf::center: value = "center"; break;
			case GridJustifySelf::stretch: value = "stretch"; break;
			}
			view->element->setStyle("justify-self", value);
		}

		if (align != GridAlignSelf::useAlignItems)
		{
			std::string value;
			switch (align)
			{
			case GridAlignSelf::useAlignItems: value = "auto"; break;
			case GridAlignSelf::start: value = "start"; break;
			case GridAlignSelf::end: value = "end"; break;
			case GridAlignSelf::center: value = "center"; break;
			case GridAlignSelf::stretch: value = "stretch"; break;
			}
			view->element->setStyle("align-self", value);
		}

		attachView(view);

		if (!sibling)
			appendChild(view->element.get());
		else
			insertBefore(view->element.get(), sibling->element.get());
	}
}
