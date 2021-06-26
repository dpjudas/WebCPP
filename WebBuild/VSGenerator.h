#pragma once

#include "Guid.h"
#include "File.h"

class VSLineWriter;
class VSSolution;
class VSCppProject;
class VSSolutionConfiguration;
class VSGenerator;

class VSLineWriter
{
public:
	void writeLine(const std::string& text);
	void save(const std::string& filename);

private:
	std::string lines;
};

class VSSolution
{
public:
	VSSolution(const std::string& name);

	void generate(std::string solutionDir);

	std::string name;
	std::string visualStudioVersion = "16.0.30621.155";
	std::string minimumVisualStudioVersion = "10.0.40219.1";
	std::string solutionGuid = "B61D85A2-54FA-468F-9672-7BF3012676B4";
	std::vector<std::unique_ptr<VSCppProject>> projects;
	std::vector<std::unique_ptr<VSSolutionConfiguration>> configurations;
};

class VSSolutionConfiguration
{
public:
	VSSolutionConfiguration(const std::string& name = "Release", const std::string& platform = "x64") : name(name), platform(platform) { }

	std::string name;
	std::string platform;
};

class VSCppProjectConfiguration
{
public:
	VSCppProjectConfiguration(const std::string& name = "Release", const std::string& platform = "x64") : name(name), platform(platform) { }

	std::string name;
	std::string platform;

	struct 
	{
		std::string configurationType = "Application";
		std::string useDebugLibraries = "false";
		std::string platformToolset = "v142";
		std::string characterSet = "Unicode";
		std::string wholeProgramOptimization = "true";
		std::string linkIncremental = "false";
		std::string outDir = "$(SolutionDir)Build\\$(Configuration)\\$(Platform)\\bin\\";
		std::string intDir = "$(SolutionDir)Build\\$(Configuration)\\$(Platform)\\obj\\";
		std::string nmakeBuildCommandLine;
		std::string nmakeOutput;
		std::string nmakeCleanCommandLine;
		std::string nmakeReBuildCommandLine;
		std::vector<std::string> nmakePreprocessorDefinitions;
		std::vector<std::string> nmakeIncludeSearchPath;
	} general;

	struct
	{
		std::string warningLevel = "Level3";
		std::string functionLevelLinking = "true";
		std::string intrinsicFunctions = "true";
		std::string sdlCheck = "true";
		std::vector<std::string> preprocessorDefinitions;
		std::string conformanceMode = "true";
		std::vector<std::string> additionalIncludeDirectories;
		std::string runtimeLibrary = "MultiThreaded";
	} clCompile;

	struct
	{
		std::string subSystem = "Console";
		std::string enableCOMDATFolding = "true";
		std::string optimizeReferences = "true";
		std::string generateDebugInformation = "true";
	} link;
};

class VSCppProjectFilter
{
public:
	VSCppProjectFilter(const std::string& name, const std::string& guid) : name(name), guid(guid) { }

	std::string name;
	std::string guid;
	std::vector<std::string> sourceFiles;
	std::vector<std::string> headerFiles;
	std::vector<std::string> extraFiles;
};

class VSCppProject
{
public:
	VSCppProject(const std::string& name) : name(name) { }

	std::string name = name;
	std::string typeGuid = "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942";
	std::string projectGuid = "B9C229A1-BDC0-4C10-973A-694146B89016";
	std::string vcProjectVersion = "16.0";
	std::string windowsTargetPlatformVersion = "10.0";
	std::vector<std::unique_ptr<VSCppProjectConfiguration>> configurations;
	std::vector<std::string> sourceFiles;
	std::vector<std::string> headerFiles;
	std::vector<std::string> extraFiles;
	std::vector<std::unique_ptr<VSCppProjectFilter>> filters;

	VSCppProjectFilter* getFilter(const std::string& name)
	{
		// create ancestors
		std::string parent = FilePath::remove_last_component(name);
		if (!parent.empty())
			getFilter(parent);

		for (auto& filter : filters)
		{
			if (filter->name == name)
				return filter.get();
		}
		std::string guid = xg::newGuid();
		filters.push_back(std::make_unique<VSCppProjectFilter>(name, guid));
		return filters.back().get();
	}
};

class VSGenerator
{
public:
	VSGenerator(std::string solutionDir) : solutionDir(std::move(solutionDir)) {}

	void writeSolution(const VSSolution* solution);
	void writeProject(const VSCppProject* project);
	void writeProjectFilters(const VSCppProject* project);

private:
	static std::string toLowerCase(std::string text);

	std::string getProjectFilename(std::string name, std::string extension);

	std::string solutionDir;
};
