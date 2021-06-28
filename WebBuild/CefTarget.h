#pragma once

#include <string>
#include <vector>

class Solution;
class Project;
class MsvcTarget;

class CefWrapperLib
{
public:
	CefWrapperLib();

	void Build(Solution* solution, std::string configuration, std::string objDir);
	void Clean(Solution* solution, std::string configuration, std::string objDir);

private:
	Project GetWrapperProject(Solution* solution, std::string configuration);
	void FindFiles(const std::string& folder, const std::string& relativeFolder);

	std::vector<std::string> InputFiles;
};

class CefCopyResources
{
public:
	CefCopyResources();

	void Build(std::string binDir);
	void Clean(std::string binDir);

private:
	void FindFiles(const std::string& folder, const std::string& relativeFolder);

	std::string CefDir;
	std::vector<std::pair<std::string, std::string>> ResourceFiles;
	std::vector<std::string> IgnoreList;
};
