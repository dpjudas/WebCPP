#pragma once

#include "../../Core/View.h"

namespace web
{
	class ListViewHeader : public View
	{
	public:
		ListViewHeader();

		void addColumn(std::string name, double width, bool expanding);
		size_t getColumnCount() const { return columns.size(); }

		double getColumnWidth(size_t i) const { return columns[i].width; }
		bool isColumnExpanding(size_t i) const { return columns[i].expanding; }

		std::vector<std::string> columnNames() const;

		std::function<void()> columnsUpdated;

	private:
		void onPointerDown(Event* e);
		void onPointerUp(Event* e);
		void onPointerCancel(Event* e);
		void onPointerMove(Event* e);

		struct Column
		{
			TextLabel* label = nullptr;
			double width = 0.0;
			bool expanding = false;
			ListViewHeaderSplitter* splitter = nullptr;
			bool visible = true;
		};

		std::vector<Column> columns;
		int movingSizer = -1;
		double moveStart = 0.0;
		double widthStart = 0.0;
	};

	class ListViewHeaderSplitter : public View
	{
	public:
		ListViewHeaderSplitter();

	private:
		View* dummy = nullptr;
	};
}
