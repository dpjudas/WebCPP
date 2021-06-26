#pragma once

#include <string>
#include <vector>

class MsvcTarget;

class CefWrapperLib
{
public:
	CefWrapperLib();

	void Build(std::string target, std::string solutionDir);
	void Clean(std::string target, std::string solutionDir);

private:
	void FindFiles(const std::string& folder, const std::string& relativeFolder);

	std::vector<std::string> InputFiles;
};

class CefCopyResources
{
public:
	CefCopyResources();

	void Build(MsvcTarget* target);
	void Clean(MsvcTarget* target);

private:
	void FindFiles(const std::string& folder, const std::string& relativeFolder);

	std::string CefDir;
	std::vector<std::pair<std::string, std::string>> ResourceFiles;
	std::vector<std::string> IgnoreList;
};
