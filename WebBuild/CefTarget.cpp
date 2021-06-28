
#include "Precomp.h"
#include "CefTarget.h"
#include "MsvcTarget.h"
#include "Environment.h"
#include "File.h"
#include "Project.h"
#include <iostream>

CefWrapperLib::CefWrapperLib()
{
	FindFiles(FilePath::combine(Environment::getVariable("EnvironmentDir"), "cef\\libcef_dll"), {});
}

Project CefWrapperLib::GetWrapperProject(Solution* solution, std::string configuration)
{
	Project p;
	p.name = "cefwrapper";
	p.type = "static-lib";
	p.output = "libcef_dll_wrapper.lib";
	p.projectDir = FilePath::combine(Environment::getVariable("EnvironmentDir"), "cef\\libcef_dll");
	p.sources = InputFiles;
	p.configurations[configuration]["cppflags"] = JsonValue::string("/O2 /Zi /DEBUG:FULL /EHsc /std:c++17 /permissive-");
	p.configurations[configuration]["libflags"] = JsonValue::string("");
	return p;
}

void CefWrapperLib::Build(Solution* solution, std::string configuration, std::string objDir)
{
	Project project = GetWrapperProject(solution, configuration);
	MsvcTarget cefwrapper;
	cefwrapper.Setup(solution, &project, configuration, objDir, objDir, {});
	cefwrapper.Build();
}

void CefWrapperLib::Clean(Solution* solution, std::string configuration, std::string objDir)
{
	Project project = GetWrapperProject(solution, configuration);
	MsvcTarget cefwrapper;
	cefwrapper.Setup(solution, &project, configuration, objDir, objDir, {});
	cefwrapper.Clean();
}

void CefWrapperLib::FindFiles(const std::string& folder, const std::string& relativeFolder)
{
	for (const std::string& name : Directory::folders(FilePath::combine(folder, "*")))
	{
		FindFiles(FilePath::combine(folder, name), FilePath::combine(relativeFolder, name));
	}

	for (const std::string& name : Directory::files(FilePath::combine(folder, "*")))
	{
		if (FilePath::has_extension(name, "cpp") || FilePath::has_extension(name, "cc"))
		{
			InputFiles.push_back(FilePath::combine(relativeFolder, name));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

CefCopyResources::CefCopyResources()
{
	CefDir = FilePath::combine(Environment::getVariable("EnvironmentDir"), "cef");
	IgnoreList = { "cef_sandbox.lib", "libcef.lib" };
	FindFiles(FilePath::combine(CefDir, "Resources"), {});
	FindFiles(FilePath::combine(CefDir, "Release"), {});
}

void CefCopyResources::Build(std::string binDir)
{
	for (const auto& file : ResourceFiles)
	{
		std::string destfilename = FilePath::combine(binDir, file.second);
		std::string destfolder = FilePath::remove_last_component(destfilename);
		auto data = File::read_all_bytes(file.first);
		if (!destfolder.empty())
			Directory::create(destfolder);
		File::write_all_bytes(destfilename, data.data(), data.size());
	}
}

void CefCopyResources::Clean(std::string binDir)
{
	for (const auto& file : ResourceFiles)
	{
		File::try_remove(FilePath::combine(binDir, file.second));
	}
}

void CefCopyResources::FindFiles(const std::string& folder, const std::string& relativeFolder)
{
	for (const std::string& name : Directory::folders(FilePath::combine(folder, "*")))
	{
		FindFiles(FilePath::combine(folder, name), FilePath::combine(relativeFolder, name));
	}

	for (const std::string& name : Directory::files(FilePath::combine(folder, "*")))
	{
		if (std::find(IgnoreList.begin(), IgnoreList.end(), name) == IgnoreList.end())
		{
			ResourceFiles.push_back({ FilePath::combine(folder, name), FilePath::combine(relativeFolder, name) });
		}
	}
}
