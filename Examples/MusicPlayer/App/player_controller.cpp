
#include "player_controller.h"
#include "audioplayer.h"
#include "audiosource.h"
#include "file.h"
#include "http_client.h"
#include "xml/xml_node.h"
#include "xml/xml_document.h"
#include <algorithm>

PlayerController::PlayerController()
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::play(const std::string& filename)
{
	PlaylistItem item;
	item.filename = filename;
	item.name = FilePath::remove_extension(FilePath::last_component(filename));
	playlist.push_back(item);

	player.reset();

	std::unique_ptr<AudioSource> source;
	if (FilePath::has_extension(filename, "mp3"))
		source = AudioSource::CreateMp3(File::read_all_bytes(filename));
	else if (FilePath::has_extension(filename, "flac"))
		source = AudioSource::CreateFlac(File::read_all_bytes(filename));
	else if (FilePath::has_extension(filename, "ogg"))
		source = AudioSource::CreateOgg(File::read_all_bytes(filename));
	else if (FilePath::has_extension(filename, "wav"))
		source = AudioSource::CreateWav(File::read_all_bytes(filename));

	if (source)
	{
		playback_info = {};
		playback_info.item = item;
		playback_info.seconds_length = source->GetSamples() / source->GetFrequency();
		player = AudioPlayer::Create(std::move(source));
	}
	else
	{
		playback_info = {};
	}
}

void PlayerController::stop()
{
	player.reset();
}

SearchFolderResult PlayerController::searchfolder(const std::string& path)
{
	SearchFolderResult result;
	result.files = Directory::files(path);
	result.folders = Directory::folders(path);

	// To do: improve this sort to take numbers into account
	std::sort(result.files.begin(), result.files.end());
	std::sort(result.folders.begin(), result.folders.end());

	auto it = result.files.begin();
	while (it != result.files.end())
	{
		const std::string& filename = *it;
		bool recognized = false;
		for (const char* extension : { "mp3", "flac", "ogg", "wav" })
		{
			if (FilePath::has_extension(filename, extension))
			{
				recognized = true;
				break;
			}
		}
		if (recognized)
			++it;
		else
			it = result.files.erase(it);
	}

	return result;
}

const std::vector<PlaylistItem>& PlayerController::get_playlist() const
{
	return playlist;
}

CurrentPlaybackInfo PlayerController::get_current_playback() const
{
	return playback_info;
}

void PlayerController::downloadPostcast()
{
	try
	{
		auto client = HttpClient::Create();
		auto rssxml = XmlDocument::load(client->GetString("http://portal-api.thisisdistorted.com/xml/chicane-presents-sun-sets"), false);
		auto channel = rssxml->named_item("rss")->named_item("channel");
		for (auto item = channel->first_child(); item; item = item->next_sibling())
		{
			if (!item->is_element() || item->name() != "item") continue;

			std::string title = item->named_item("title")->text();
			std::string url = item->named_item("enclosure")->attribute("url");
			std::string type = item->named_item("enclosure")->attribute("type");
			if (type == "audio/mpeg" || type == "audio/mp3")
			{
				/*std::string filename = "\\\\nas\\music\\Chicane Sunsets\\" + StringReplace(title, ":", "") + ".mp3";
				if (!File.Exists(filename))
				{
					Console.WriteLine("Downloading {0}", title);
					byte[] data = client.GetByteArrayAsync(url).Result;
					File.WriteAllBytes(filename, data);
				}
				else
				{
					Console.WriteLine("Skipping {0} (already downloaded)", title);
				}*/
			}
			else
			{
				//Console.WriteLine("Skipping {0} ({1})", title, type);
			}
		}
	}
	catch (const std::exception& e)
	{
		(void)e;
	}
}
