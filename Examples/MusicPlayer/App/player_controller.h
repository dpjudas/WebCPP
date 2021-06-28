
#pragma once

#include <memory>
#include <string>
#include <vector>

class AudioPlayer;

struct SearchFolderResult
{
	std::vector<std::string> files;
	std::vector<std::string> folders;
};

struct PlaylistItem
{
	std::string name;
	std::string filename;
};

struct CurrentPlaybackInfo
{
	PlaylistItem item;
	int seconds_position = 0;
	int seconds_length = 0;
};

class PlayerController
{
public:
	PlayerController();
	~PlayerController();

	void play(const std::string& filename);
	void stop();

	const std::vector<PlaylistItem>& get_playlist() const;
	CurrentPlaybackInfo get_current_playback() const;

	SearchFolderResult searchfolder(const std::string& path);

	void downloadPostcast();

private:
	std::unique_ptr<AudioPlayer> player;
	std::vector<PlaylistItem> playlist;
	CurrentPlaybackInfo playback_info;
};
