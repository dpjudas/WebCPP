#pragma once

#include <memory>
#include <vector>
#include <list>
#include <string>

namespace web
{
	class View;
	class ViewLayout;
	class Element;

	class ViewLayoutItem
	{
	public:
		virtual ~ViewLayoutItem() = default;
		virtual std::string getStyles() = 0;

		ViewLayout* layout = nullptr;
		std::shared_ptr<View> view;
		std::list<std::unique_ptr<ViewLayoutItem>>::iterator it;
		int classNameIndex = -1;
	};

	class ViewLayout
	{
	public:
		ViewLayout(View* owner);
		virtual ~ViewLayout();

		void clear();

		void addStickyView(std::shared_ptr<View> view);
		void addAbsoluteView(std::shared_ptr<View> view);
		void addFixedView(std::shared_ptr<View> view);

		virtual std::string getStyles() = 0;

	protected:
		void attachView(std::shared_ptr<View> view, std::unique_ptr<ViewLayoutItem> item);
		void appendChild(Element* element);
		void insertBefore(Element* newElement, Element* insertPoint);
		void removeChild(Element* element);

		View* owner = nullptr;

	private:
		void detachItem(ViewLayoutItem* item);
		bool applyDefaultFocus();
		bool focusFirstItem();

		std::list<std::unique_ptr<ViewLayoutItem>> items;

		friend class View;
	};

	//////////////////////////////////////////////////////////////////////////////

	enum class FlowPosition
	{
		unspecified,
		sticky,
		absolute,
		fixed
	};

	class FlowLayoutItem : public ViewLayoutItem
	{
	public:
		FlowLayoutItem(FlowPosition position) : position(position) {}
		std::string getStyles() override;

		FlowPosition position = {};
	};

	class FlowLayout : public ViewLayout
	{
	public:
		FlowLayout(View* owner);
		~FlowLayout();

		void addView(std::shared_ptr<View> view);

		std::string getStyles() override;
	};

	//////////////////////////////////////////////////////////////////////////////

	enum class FlexDirection
	{
		unspecified,
		row,
		rowReverse,
		column,
		columnReverse
	};

	enum class FlexWrap
	{
		unspecified,
		nowrap,
		wrap,
		wrapReverse
	};

	enum class FlexJustifyContent
	{
		unspecified,
		flexStart,
		flexEnd,
		center,
		spaceBetween,
		spaceAround,
		spaceEvenly
	};

	enum class FlexAlignItems
	{
		unspecified,
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
		FlexLayoutItem(bool grow, bool shrink, FlexAlignSelf alignSelf, int order) : grow(grow), shrink(shrink), alignSelf(alignSelf), order(order) {}
		std::string getStyles() override;
		bool grow = false;
		bool shrink = false;
		FlexAlignSelf alignSelf = {};
		int order = 0;
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

		void addView(std::shared_ptr<View> view, bool grow = false, bool shrink = false, FlexAlignSelf align = FlexAlignSelf::useAlignItems, int order = 0) { addViewBefore(view, nullptr, grow, shrink, align, order); }
		void addViewBefore(std::shared_ptr<View> view, std::shared_ptr<View> sibling, bool grow = false, bool shrink = false, FlexAlignSelf align = FlexAlignSelf::useAlignItems, int order = 0);
		void addSpacer();

		std::string getStyles() override;

	private:
		FlexDirection direction = {};
		FlexWrap wrap = {};
		FlexJustifyContent justifyContent = {};
		FlexAlignItems alignItems = {};
		struct
		{
			double rowLength = 0.0;
			double columnLength = 0.0;
		} gap;
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
		std::string getStyles() override;
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
		void addView(std::shared_ptr<View> view, int column = 0, int row = 0, int colspan = 1, int rowspan = 1, GridJustifySelf justify = GridJustifySelf::useJustifyItems, GridAlignSelf align = GridAlignSelf::useAlignItems) { addViewBefore(view, nullptr, column, row, colspan, rowspan, justify, align); }
		void addViewBefore(std::shared_ptr<View> view, std::shared_ptr<View> sibling, int column = 0, int row = 0, int colspan = 1, int rowspan = 1, GridJustifySelf justify = GridJustifySelf::useJustifyItems, GridAlignSelf align = GridAlignSelf::useAlignItems);

		std::string getStyles() override;

	private:
		static std::string toString(const std::vector<GridTrackSize>& sizes);
	};
}
