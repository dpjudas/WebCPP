#pragma once

#include <WebCPP/Core/View.h>

class TextLabel;
class ListView;
class ListViewItem;

class Playlist : public VBoxView
{
public:
	Playlist(View* parent);

	void update();

private:
	void onItemClicked(ListViewItem* item);

	TextLabel* title = nullptr;
	ListView* listview = nullptr;
};
