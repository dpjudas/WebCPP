#pragma once

#include <WebCPP/Core/View.h>

class MusicPlayer;
class TextLabel;
class ListView;
class ListViewItem;

class NavPanel : public VBoxView
{
public:
	NavPanel(MusicPlayer* musicplayer, View* parent);

private:
	void updateFolder(ListViewItem* item);
	void onActivate(ListViewItem* item);

	MusicPlayer* musicplayer = nullptr;
	TextLabel* title = nullptr;
	ListView* listview = nullptr;
};
