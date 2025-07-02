
#include "WebCPP/Controls/ListView/TextListViewItem.h"

namespace web
{
	TextListViewItem::TextListViewItem(std::string id)
	{
		setId(id);
	}

	TextListViewItem::TextListViewItem(std::string id, std::string icon)
	{
		setId(id);
		setIcon(0, std::move(icon));
	}

	TextListViewItem::TextListViewItem(std::string id, std::vector<std::string> columns) : columns(std::move(columns))
	{
		setId(std::move(id));
	}

	TextListViewItem::TextListViewItem(std::string id, std::string icon, std::vector<std::string> columns, bool editable, bool removable) : columns(std::move(columns))
	{
		setIcon(0, std::move(icon));
		setId(std::move(id));
		setEditable(editable);
		setRemovable(removable);
	}

	TextListViewItem::TextListViewItem(std::string id, const std::vector<std::pair<std::string, std::string>>& columns, bool editable, bool removable)
	{
		std::vector<std::string> icos, cols;
		for (const auto& pair : columns)
		{
			icos.push_back(pair.first);
			cols.push_back(pair.second);
		}
		this->icons = std::move(icos);
		this->columns = std::move(cols);

		setId(std::move(id));
		setEditable(editable);
		setRemovable(removable);
	}

	TextListViewItem::TextListViewItem(std::vector<std::string> columns) : columns(std::move(columns))
	{
	}

	void TextListViewItem::setIcon(size_t index, std::string src)
	{
		if (index >= icons.size())
			icons.resize(index + 1);
		icons[index] = std::move(src);
		updateColumn(index);
	}

	void TextListViewItem::setText(size_t index, std::string text)
	{
		if (index >= columns.size())
			columns.resize(index + 1);
		columns[index] = text;
		updateColumn(index);
	}

	std::string TextListViewItem::getIcon(size_t index) const
	{
		if (index < icons.size())
			return icons[index];
		else
			return {};
	}

	std::string TextListViewItem::getText(size_t index) const
	{
		if (index < columns.size())
			return columns[index];
		else
			return {};
	}

	bool TextListViewItem::isEditable() const
	{
		return editable;
	}

	void TextListViewItem::setEditable(bool value)
	{
		editable = value;
	}

	bool TextListViewItem::isRemovable() const
	{
		return removable;
	}

	void TextListViewItem::setRemovable(bool value)
	{
		removable = value;
	}

	void TextListViewItem::updateColumnView(size_t index)
	{
		itemview()->setColumnView(index, createTextColumn(getIcon(index), getText(index)));
	}

	std::string TextListViewItem::sortCompareString()
	{
		if (!columns.empty())
			return columns.front();
		else
			return {};
	}

	std::shared_ptr<View> TextListViewItem::createTextColumn(const std::string& icon, const std::string& text)
	{
		if (!icon.empty())
		{
			auto view = std::make_shared<View>();
			auto image = std::make_shared<ImageBox>();
			auto label = std::make_shared<TextLabel>();
			image->setSrc(icon);
			image->addClass("listviewitemicon");
			label->setText(text);
			auto layout = view->createHBoxLayout();
			layout->addView(image);
			layout->addView(label, false, true);
			return view;
		}
		else
		{
			auto view = std::make_shared<TextLabel>();
			view->setText(text);
			return view;
		}
	}
}
