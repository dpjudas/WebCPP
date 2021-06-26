
#include "Precomp.h"
#include "Solution.h"
#include "Project.h"
#include "File.h"
#include "JsonValue.h"
#include <regex>
#include <iostream>

Solution Solution::load(const std::string& filename)
{
	JsonValue json = JsonValue::parse(File::read_all_text(filename));

	Solution s;
	s.solutionDir = FilePath::remove_last_component(filename);
	
	for (auto& it : json["environment"].properties())
	{
		s.environment[it.first] = it.second;
	}

	for (auto& item : json["projects"].items())
	{
		if (item.type() == JsonType::string)
		{
			Project p = Project::load(FilePath::combine(s.solutionDir, item.to_string()));
			s.projects[p.name] = std::move(p);
		}
	}

	return s;
}

std::string Solution::getValue(const std::string& name, const Project* project, const std::string& configuration, const std::string& defaultValue) const
{
	auto itConfig = project->configurations.find(configuration);
	if (itConfig == project->configurations.end())
		return defaultValue;

	auto itValue = itConfig->second.find(name);
	if (itValue == itConfig->second.end() || !itValue->second.is_string())
		return defaultValue;

	std::string value = itValue->second.to_string();

	static std::regex expression("\\$\\((\\w+)\\)");
	std::smatch match;
	while (std::regex_search(value, match, expression))
	{
		std::string prefix = match.prefix();
		std::string suffix = match.suffix();
		std::string variable = match[1];

		if (variable == "SolutionDir")
		{
			variable = solutionDir;
		}
		else if (variable == "ProjectDir")
		{
			variable = project->projectDir;
		}
		else
		{
			variable = "";
		}

		value = prefix + variable + suffix;
	}

	return value;
}
