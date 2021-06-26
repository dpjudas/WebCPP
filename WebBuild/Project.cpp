
#include "Precomp.h"
#include "Project.h"
#include "File.h"
#include "JsonValue.h"
#include <iostream>

Project Project::load(const std::string& filename)
{
	JsonValue json = JsonValue::parse(File::read_all_text(filename));

	Project p;
	p.projectDir = FilePath::remove_last_component(filename);
	p.name = json["name"].to_string();
	p.type = json["type"].to_string();
	p.output = json["output"].to_string();
	p.wwwroot = json["wwwroot"].to_string();
	p.cssfile = json["cssfile"].to_string();
	p.shellfile = json["shellfile"].to_string();
	p.prejs = json["prejs"].to_string();

	for (const JsonValue& item : json["sources"].items())
	{
		if (item.is_string())
			p.sources.push_back(item.to_string());
	}

	for (const JsonValue& configItem : json["configurations"].items())
	{
		if (configItem.is_object())
		{
			std::string name;
			std::map<std::string, JsonValue> properties;
			for (auto& it : configItem.properties())
			{
				if (it.first == "name")
				{
					name = it.second.to_string();
				}
				else
				{
					properties[it.first] = it.second;
				}
			}
			if (!name.empty())
				p.configurations[name] = std::move(properties);
		}
	}

	return p;
}

std::vector<std::string> Project::getFilteredSources(const std::vector<std::string> extensions) const
{
	std::vector<std::string> results;
	for (const std::string& filename : sources)
	{
		for (const std::string& ext : extensions)
		{
			if (FilePath::has_extension(filename, ext.c_str()))
			{
				results.push_back(filename);
				break;
			}
		}
	}
	return results;
}
