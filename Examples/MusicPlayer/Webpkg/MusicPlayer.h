#pragma once

#include <WebCPP/Core/DocumentBodyView.h>

class WindowHeader;
class Menubar;
class Menu;
class Playlist;
class PodcastEpisode;
class PlayerPanel;
class BorderLayoutView;

class MusicPlayer : public DocumentBodyView
{
public:
	MusicPlayer();

	PlayerPanel* player = nullptr;
	Playlist* playlist = nullptr;
	PodcastEpisode* episode = nullptr;

private:
	void onFileMenu(Menu* menu);
	void onEditMenu(Menu* menu);
	void onViewMenu(Menu* menu);
	void onPlaybackMenu(Menu* menu);
	void onLibraryMenu(Menu* menu);
	void onHelpMenu(Menu* menu);

	WindowHeader* header = nullptr;
	Menubar* menubar = nullptr;
	BorderLayoutView* borderlayout = nullptr;
};
