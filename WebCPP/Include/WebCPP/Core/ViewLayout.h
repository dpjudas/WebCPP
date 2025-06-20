#pragma once

#include <memory>
#include <vector>

namespace web
{
	class View;
	class Element;

	class ViewLayoutItem
	{
	public:
		virtual ~ViewLayoutItem() = default;
	};

	class ViewLayout
	{
	public:
		ViewLayout(View* owner);
		virtual ~ViewLayout() = default;

		void addStickyView(View* view);
		void addAbsoluteView(View* view);
		void addFixedView(View* view);

	protected:
		virtual void onViewRemoved(View* view);

		ViewLayoutItem* getItem(View* view) { return getItemImpl(view); }
		template<typename T> T* getItem(View* view) { return static_cast<T*>(getItemImpl(view)); }
		void setItem(View* view, std::unique_ptr<ViewLayoutItem> item);
		void resetItem(View* view);

		void appendChild(Element* element);
		void insertBefore(Element* newElement, Element* insertPoint);
		void removeChild(Element* element);

		View* owner = nullptr;

	private:
		ViewLayoutItem* getItemImpl(View* view);

		friend class View;
	};

	//////////////////////////////////////////////////////////////////////////////

	class FlowLayout : public ViewLayout
	{
	public:
		FlowLayout(View* owner);
		~FlowLayout();

		void addView(View* view);
	};

	//////////////////////////////////////////////////////////////////////////////

	enum class FlexDirection
	{
		row,
		rowReverse,
		column,
		columnReverse
	};

	enum class FlexWrap
	{
		nowrap,
		wrap,
		wrapReverse
	};

	enum class FlexJustifyContent
	{
		flexStart,
		flexEnd,
		center,
		spaceBetween,
		spaceAround,
		spaceEvenly
	};

	enum class FlexAlignItems
	{
		flexStart,
		flexEnd,
		center,
		stretch,
		baseline
	};

	enum class FlexAlignSelf
	{
		useAlignItems,
		flexStart,
		flexEnd,
		center,
		stretch,
		baseline
	};

	class FlexLayoutItem : public ViewLayoutItem
	{
	public:
	};

	class FlexLayout : public ViewLayout
	{
	public:
		FlexLayout(View* owner);
		~FlexLayout();

		void setDirection(FlexDirection direction);
		void setWrap(FlexWrap wrap);
		void setJustifyContent(FlexJustifyContent justify);
		void setAlignItems(FlexAlignItems align);
		void setGap(double length);
		void setGap(double rowLength, double columnLength);

		void addView(View* view, bool grow = false, bool shrink = false, FlexAlignSelf align = FlexAlignSelf::useAlignItems, int order = 0) { addViewBefore(view, nullptr, grow, shrink, align, order); }
		void addViewBefore(View* view, View* sibling, bool grow = false, bool shrink = false, FlexAlignSelf align = FlexAlignSelf::useAlignItems, int order = 0);
		void addSpacer();

	private:
		std::vector<std::unique_ptr<Element>> spacers;
	};

	//////////////////////////////////////////////////////////////////////////////

	class VBoxLayout : public FlexLayout
	{
	public:
		VBoxLayout(View* owner) : FlexLayout(owner)
		{
			setDirection(FlexDirection::column);
		}
	};

	//////////////////////////////////////////////////////////////////////////////

	class HBoxLayout : public FlexLayout
	{
	public:
		HBoxLayout(View* owner) : FlexLayout(owner)
		{
			setDirection(FlexDirection::row);
		}
	};

	//////////////////////////////////////////////////////////////////////////////

	class GridLayoutItem : public ViewLayoutItem
	{
	public:
	};

	enum class GridJustifyItems
	{
		start,
		end,
		center,
		stretch
	};

	enum class GridAlignItems
	{
		stretch,
		start,
		end,
		center,
		baseline
	};

	enum class GridJustifyContent
	{
		start,
		end,
		center,
		stretch,
		spaceAround,
		spaceBetween,
		spaceEvenly
	};

	enum class GridAlignContent
	{
		start,
		end,
		center,
		stretch,
		spaceAround,
		spaceBetween,
		spaceEvenly
	};

	enum class GridAutoFlow
	{
		row,
		column,
		rowDense,
		columnDense
	};

	enum class GridJustifySelf
	{
		useJustifyItems,
		start,
		end,
		center,
		stretch
	};

	enum class GridAlignSelf
	{
		useAlignItems,
		start,
		end,
		center,
		stretch
	};

	class GridTrackSize
	{
	public:
		GridTrackSize(std::string value) : value(std::move(value)) {}
		GridTrackSize(double v, std::string unit = "px") { value = std::to_string(v) + unit; }

		std::string value;
	};

	class GridLayout : public ViewLayout
	{
	public:
		GridLayout(View* owner);

		static const GridTrackSize autoSize;
		static const GridTrackSize minContentSize;
		static const GridTrackSize maxContentSize;
		static const GridTrackSize fitContentSize;
		static const GridTrackSize minSize;
		static const GridTrackSize maxSize;
		static const GridTrackSize freeSpace;
		static GridTrackSize minmaxSize(GridTrackSize minval, GridTrackSize maxval);

		void setColumns(const std::vector<GridTrackSize>& sizes);
		void setRows(const std::vector<GridTrackSize>& sizes);
		void setAutoColumns(const std::vector<GridTrackSize>& sizes);
		void setAutoRows(const std::vector<GridTrackSize>& sizes);
		void setAutoFlow(GridAutoFlow flow);
		void setSubgrid(bool column, bool row);
		void setGap(double length);
		void setGap(double rowLength, double columnLength);
		void setJustifyItems(GridJustifyItems justify);
		void setAlignItems(GridAlignItems align);
		void setJustifyContent(GridJustifyContent justify);
		void setAlignContent(GridAlignContent align);

		// Note: column and row index from one, not zero. Zero means auto placement. Negative values starts from the end of the grid.
		void addView(View* view, int column = 0, int row = 0, int colspan = 1, int rowspan = 1, GridJustifySelf justify = GridJustifySelf::useJustifyItems, GridAlignSelf align = GridAlignSelf::useAlignItems) { addViewBefore(view, nullptr, column, row, colspan, rowspan, justify, align); }
		void addViewBefore(View* view, View* sibling, int column = 0, int row = 0, int colspan = 1, int rowspan = 1, GridJustifySelf justify = GridJustifySelf::useJustifyItems, GridAlignSelf align = GridAlignSelf::useAlignItems);

	private:
		static std::string toString(const std::vector<GridTrackSize>& sizes);
	};
}
