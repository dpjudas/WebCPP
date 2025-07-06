
#include "WebCPP/Core/ViewLayout.h"
#include "WebCPP/Core/View.h"
#include "WebCPP/Core/Element.h"
#include "WebCPP/Core/ShadowRoot.h"
#include "WebCPP/Core/StyleSheet.h"
#include <unordered_map>

namespace web
{
	class LayoutStyles
	{
	public:
		static LayoutStyles* get()
		{
			static LayoutStyles styles;
			return &styles;
		}

		void addStyles(ViewLayoutItem* item)
		{
			std::string styles;
			if (auto layout = item->view->getLayout())
				styles += layout->getStyles();
			if (!styles.empty() && styles.back() != ' ')
				styles.push_back(' ');
			styles += item->getStyles();
			if (!styles.empty() && styles.back() != ' ')
				styles.push_back(' ');

			if (styles.empty())
				return;

			auto it = stylesToNameIndex.find(styles);
			if (it != stylesToNameIndex.end())
			{
				item->classNameIndex = it->second;
			}
			else
			{
				int index = (int)classNames.size();
				std::string className = "layout-" + std::to_string(index);

				StyleSheet::insertRule("." + className + "{ " + styles + " }");

				classNames.push_back(className);
				stylesToNameIndex.emplace(styles, index);

				item->classNameIndex = index;
			}

			item->view->addClass(classNames[item->classNameIndex]);
		}

		void removeStyles(ViewLayoutItem* item)
		{
			if (item->classNameIndex != -1)
				item->view->removeClass(classNames[item->classNameIndex]);
			item->classNameIndex = -1;
		}

		std::unordered_map<std::string, int> stylesToNameIndex;
		std::vector<std::string> classNames;
	};

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
		LayoutStyles::get()->removeStyles(item);
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
		attachView(view, std::make_unique<FlowLayoutItem>(FlowPosition::sticky));
		appendChild(view->element.get());
	}

	void ViewLayout::addAbsoluteView(std::shared_ptr<View> view)
	{
		attachView(view, std::make_unique<FlowLayoutItem>(FlowPosition::absolute));
		appendChild(view->element.get());
	}

	void ViewLayout::addFixedView(std::shared_ptr<View> view)
	{
		attachView(view, std::make_unique<FlowLayoutItem>(FlowPosition::fixed));
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

	void ViewLayout::attachView(std::shared_ptr<View> view, std::unique_ptr<ViewLayoutItem> item)
	{
		view->detach();

		items.push_back(std::move(item));
		items.back()->it = --items.end();
		items.back()->layout = this;
		items.back()->view = std::move(view);
		items.back()->view->layoutItem = items.back().get();

		LayoutStyles::get()->addStyles(items.back().get());
	}

	//////////////////////////////////////////////////////////////////////////////

	std::string FlowLayoutItem::getStyles()
	{
		switch (position)
		{
		default:
		case FlowPosition::unspecified:
			return {};
		case FlowPosition::sticky:
			return "position: sticky;";
		case FlowPosition::absolute:
			return "position: absolute;";
		case FlowPosition::fixed:
			return "position: fixed;";
		}
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
		attachView(view, std::make_unique<FlowLayoutItem>(FlowPosition::unspecified));
		appendChild(view->element.get());
	}

	std::string FlowLayout::getStyles()
	{
		return {};
	}

	//////////////////////////////////////////////////////////////////////////////

	std::string FlexLayoutItem::getStyles()
	{
		std::string styles;
		if (grow && shrink)
		{
			styles += "flex: 1 1 auto; min-width: 0; min-height: 0;";
		}
		else if (grow)
		{
			styles += "flex: 1 0 auto;";
		}
		else if (shrink)
		{
			styles += "flex: 0 1 auto; min-width: 0; min-height: 0;";
		}
		else
		{
			styles += "flex: 0 0 auto;";
		}

		std::string value;
		switch (alignSelf)
		{
		case FlexAlignSelf::useAlignItems: /* value = "auto"; */ break;
		case FlexAlignSelf::flexStart: value = "flex-start"; break;
		case FlexAlignSelf::flexEnd: value = "flex-end"; break;
		case FlexAlignSelf::center: value = "center"; break;
		case FlexAlignSelf::stretch: value = "stretch"; break;
		case FlexAlignSelf::baseline: value = "baseline"; break;
		}
		if (!value.empty())
			styles += "align-self: " + value + ";";

		if (order != 0)
			styles += "order: " + std::to_string(order) + ";";

		return styles;
	}

	//////////////////////////////////////////////////////////////////////////////

	FlexLayout::FlexLayout(View* owner) : ViewLayout(owner)
	{
	}

	FlexLayout::~FlexLayout()
	{
	}

	void FlexLayout::setDirection(FlexDirection value)
	{
		direction = value;
	}

	void FlexLayout::setWrap(FlexWrap value)
	{
		wrap = value;
	}

	void FlexLayout::setJustifyContent(FlexJustifyContent value)
	{
		justifyContent = value;
	}

	void FlexLayout::setAlignItems(FlexAlignItems align)
	{
		alignItems = align;
	}

	void FlexLayout::setGap(double length)
	{
		setGap(length, length);
	}

	void FlexLayout::setGap(double rowLength, double columnLength)
	{
		gap.rowLength = rowLength;
		gap.columnLength = columnLength;
	}

	void FlexLayout::addViewBefore(std::shared_ptr<View> view, std::shared_ptr<View> sibling, bool grow, bool shrink, FlexAlignSelf align, int order)
	{
		if (sibling && sibling->parent() != owner)
			sibling = nullptr;

		attachView(view, std::make_unique<FlexLayoutItem>(grow, shrink, align, order));

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

	std::string FlexLayout::getStyles()
	{
		std::string styles = "display: flex;";

		std::string value;
		switch (direction)
		{
		case FlexDirection::unspecified: break;
		case FlexDirection::row: value = "row"; break;
		case FlexDirection::rowReverse: value = "rowReverse"; break;
		case FlexDirection::column: value = "column"; break;
		case FlexDirection::columnReverse: value = "columnReverse"; break;
		}
		if (!value.empty())
			styles += "flex-direction: " + value + ";";

		value.clear();
		switch (wrap)
		{
		case FlexWrap::unspecified: break;
		case FlexWrap::nowrap: value = "nowrap"; break;
		case FlexWrap::wrap: value = "wrap"; break;
		case FlexWrap::wrapReverse: value = "wrap-reverse"; break;
		}
		if (!value.empty())
			styles += "flex-wrap: " + value + ";";

		value.clear();
		switch (justifyContent)
		{
		case FlexJustifyContent::unspecified: break;
		case FlexJustifyContent::flexStart: value = "flex-start"; break;
		case FlexJustifyContent::flexEnd: value = "flex-end"; break;
		case FlexJustifyContent::center: value = "center"; break;
		case FlexJustifyContent::spaceBetween: value = "space-between"; break;
		case FlexJustifyContent::spaceAround: value = "space-around"; break;
		case FlexJustifyContent::spaceEvenly: value = "space-evenly"; break;
		}
		if (!value.empty())
			styles += "justify-content: " + value + ";";

		value.clear();
		switch (alignItems)
		{
		case FlexAlignItems::unspecified: break;
		case FlexAlignItems::flexStart: value = "flex-start"; break;
		case FlexAlignItems::flexEnd: value = "flex-end"; break;
		case FlexAlignItems::center: value = "center"; break;
		case FlexAlignItems::stretch: value = "stretch"; break;
		case FlexAlignItems::baseline: value = "baseline"; break;
		}
		if (!value.empty())
			styles += "align-items: " + value + ";";

		if (gap.columnLength != 0.0 || gap.rowLength != 0.0)
		{
			if (gap.columnLength == gap.rowLength)
				styles += "gap: " + std::to_string(gap.columnLength) + "px;";
			else
				styles += "gap: " + std::to_string(gap.rowLength) + " " + std::to_string(gap.columnLength) + "px;";
		}

		return styles;
	}

	//////////////////////////////////////////////////////////////////////////////

	std::string GridLayoutItem::getStyles()
	{
		return {};
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

		attachView(view, std::make_unique<GridLayoutItem>());

		if (!sibling)
			appendChild(view->element.get());
		else
			insertBefore(view->element.get(), sibling->element.get());
	}

	std::string GridLayout::getStyles()
	{
		return "display: grid;";
	}
}
