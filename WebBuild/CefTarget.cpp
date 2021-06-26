
#include "Precomp.h"
#include "CefTarget.h"
#include "MsvcTarget.h"
#include "Environment.h"
#include "File.h"

CefWrapperLib::CefWrapperLib()
{
	FindFiles(FilePath::combine(Environment::getVariable("EnvironmentDir"), "cef\\libcef_dll"), {});
}

void CefWrapperLib::Build(std::string target, std::string solutionDir)
{
	MsvcTarget cefwrapper;
	cefwrapper.InputFiles = InputFiles;
	cefwrapper.Setup(target, "libcef_dll_wrapper.lib", FilePath::combine(Environment::getVariable("EnvironmentDir"), "cef\\libcef_dll"), solutionDir);
	cefwrapper.Build();
}

void CefWrapperLib::Clean(std::string target, std::string solutionDir)
{
	MsvcTarget cefwrapper;
	cefwrapper.InputFiles = InputFiles;
	cefwrapper.Setup(target, "libcef_dll_wrapper.lib", FilePath::combine(Environment::getVariable("EnvironmentDir"), "cef\\libcef_dll"), solutionDir);
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

void CefCopyResources::Build(MsvcTarget* target)
{
	for (const auto& file : ResourceFiles)
	{
		std::string destfilename = FilePath::combine(target->binDir, file.second);
		std::string destfolder = FilePath::remove_last_component(destfilename);
		auto data = File::read_all_bytes(file.first);
		if (!destfolder.empty())
			Directory::create(destfolder);
		File::write_all_bytes(destfilename, data.data(), data.size());
	}
}

void CefCopyResources::Clean(MsvcTarget* target)
{
	for (const auto& file : ResourceFiles)
	{
		File::try_remove(FilePath::combine(target->binDir, file.second));
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
