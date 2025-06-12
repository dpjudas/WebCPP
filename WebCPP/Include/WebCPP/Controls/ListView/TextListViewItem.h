#pragma once

#include "ListViewItem.h"

class TextListViewItem : public ListViewItem
{
public:
	TextListViewItem() = default;
	TextListViewItem(std::string id);
	TextListViewItem(std::string id, std::string icon);
	TextListViewItem(std::string id, std::vector<std::string> columns);
	TextListViewItem(std::string id, std::string icon, std::vector<std::string> columns, bool editable = true, bool removable = true);
	TextListViewItem(std::string id, const std::vector<std::pair<std::string, std::string>>& columns, bool editable = true, bool removable = true);
	TextListViewItem(std::vector<std::string> columns);

	void setIcon(size_t index, std::string src);
	void setText(size_t index, std::string text);

	std::string getIcon(size_t index) const;
	std::string getText(size_t index) const;

	bool isEditable() const;
	void setEditable(bool value);

	bool isRemovable() const;
	void setRemovable(bool value);

protected:
	void updateColumnView(size_t index) override;
	std::string sortCompareString() override;

private:
	View* createTextColumn(const std::string& icon, const std::string& text);

	std::vector<std::string> icons;
	std::vector<std::string> columns;
	bool editable = true;
	bool removable = true;
};
