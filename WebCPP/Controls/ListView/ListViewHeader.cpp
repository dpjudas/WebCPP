
#include "WebCPP/Controls/ListView/ListView.h"
#include "WebCPP/Controls/ListView/ListViewItem.h"
#include "WebCPP/Controls/ListView/ListViewHeader.h"

namespace web
{
	ListViewHeader::ListViewHeader() : View("listviewheader-view")
	{
		createHBoxLayout();

		element->addEventListener("pointerdown", [=](Event* e) { onPointerDown(e); });
		element->addEventListener("pointerup", [=](Event* e) { onPointerUp(e); });
		element->addEventListener("pointercancel", [=](Event* e) { onPointerCancel(e); });
		element->addEventListener("pointermove", [=](Event* e) { onPointerMove(e); });
	}

	void ListViewHeader::addColumn(std::string name, double width, bool expanding)
	{
		Column col;
		if (!columns.empty())
		{
			columns.back().splitter = new ListViewHeaderSplitter();
			getLayout<HBoxLayout>()->addView(columns.back().splitter);
		}

		col.label = new TextLabel();
		col.label->addClass("listview-headerlabel");
		col.label->setText(name);
		col.label->element->setStyle("width", std::to_string(width) + "px");

		col.width = width;
		col.expanding = expanding;

		getLayout<HBoxLayout>()->addView(col.label, expanding);

		columns.push_back(col);
	}

	std::vector<std::string> ListViewHeader::columnNames() const
	{
		std::vector<std::string> names;
		for (const ListViewHeader::Column& column : columns)
			names.push_back(column.label->getText());
		return names;
	}

	void ListViewHeader::onPointerDown(Event* e)
	{
		e->stopPropagation();
		e->preventDefault();

		element->setPointerCapture(e->pointerId());

		moveStart = e->handle["clientX"].as<double>();
		movingSizer = -1;

		for (size_t i = 0; i < columns.size(); i++)
		{
			Column& col = columns[i];
			if (col.splitter)
			{
				Rect box = col.splitter->element->getBoundingClientRect();
				if (box.x <= moveStart && box.x + box.width > moveStart)
				{
					if (col.expanding)
					{
						// Expanding columns are automatically calculated by the browser.
						// If the user is dragging it then turn off automatic calculations and use current width as the new fixed width:
						col.expanding = false;
						col.width = col.label->element->getBoundingClientRect().width;

						// Recreate the label
						std::string name = col.label->getText();
						delete col.label;
						col.label = new TextLabel();
						col.label->addClass("listview-headerlabel");
						col.label->setText(name);
						col.label->element->setStyle("width", std::to_string(col.width) + "px");
						getLayout<HBoxLayout>()->addViewBefore(col.label, col.splitter);
					}

					movingSizer = (int)i;
					widthStart = col.width;
					addClass("sizingmode");
					break;
				}
			}
		}
	}

	void ListViewHeader::onPointerUp(Event* e)
	{
		if (movingSizer != -1)
			removeClass("sizingmode");
		movingSizer = -1;
		e->stopPropagation();
		e->preventDefault();
	}

	void ListViewHeader::onPointerCancel(Event* e)
	{
		if (movingSizer != -1)
			removeClass("sizingmode");
		movingSizer = -1;
		e->stopPropagation();
	}

	void ListViewHeader::onPointerMove(Event* e)
	{
		if (movingSizer != -1)
		{
			double movePos = e->handle["clientX"].as<double>();
			double moveDelta = movePos - moveStart;

			Column& col = columns[movingSizer];
			col.width = std::max(widthStart + moveDelta, 16.0);
			col.label->element->setStyle("width", std::to_string(col.width) + "px");

			if (columnsUpdated)
				columnsUpdated();
		}
	}

	/////////////////////////////////////////////////////////////////////////////

	ListViewHeaderSplitter::ListViewHeaderSplitter() : View("listviewheadersplitter-view")
	{
		element->setStyle("width", "10px");
		dummy = new View();
		auto layout = createHBoxLayout();
		layout->addView(dummy);
	}
}
