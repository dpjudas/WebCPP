
#include "Precomp.h"
#include "MsvcTarget.h"
#include "File.h"
#include "Environment.h"
#include "ZipWriter.h"
#include "JsonValue.h"
#include "Solution.h"
#include "Project.h"
#include <iostream>

void MsvcTarget::Setup(Solution* solution, Project* project, std::string configuration, std::string initbinDir, std::string initobjDir, std::string wrapperObjDir)
{
	msvcDir = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.29.30037\\bin\\Hostx64\\x64";
	windowsSdkDir = "C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.18362.0\\x64";
	cl = "\"" + FilePath::combine(msvcDir, "cl") + "\"";
	link = "\"" + FilePath::combine(msvcDir, "link") + "\"";
	lib = "\"" + FilePath::combine(msvcDir, "lib") + "\"";
	rc = "\"" + FilePath::combine(windowsSdkDir, "rc") + "\"";

	CppFiles = project->getFilteredSources({ "cpp", "cc", "ixx" });
	RcFiles = project->getFilteredSources({ "rc" });
	ManifestFiles = project->getFilteredSources({ "manifest" });

	srcDir = project->projectDir;
	objDir = initobjDir;
	binDir = initbinDir;

	std::string environmentDir = Environment::getVariable("EnvironmentDir");
	std::string cef = FilePath::combine(environmentDir, "cef");
	std::string libpath = FilePath::combine(cef, "Release");

	compileFlags = "-I \"" + cef + "\" /DWIN32 /D_WIN32 /D_WINDOWS /DWINVER=0x0601 /D_WIN32_WINNT=0x601 /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN /DCEF_USE_SANDBOX /DCEF_USE_ATL /DWRAPPING_CEF_SHARED " + solution->getValue("cppflags", project, configuration);
	linkFlags = "user32.lib gdi32.lib ole32.lib advapi32.lib version.lib winmm.lib propsys.lib setupapi.lib dbghelp.lib powrprof.lib shell32.lib ws2_32.lib shlwapi.lib delayimp.lib /LIBPATH:\"" + libpath + "\" libcef.lib cef_sandbox.lib " + solution->getValue("linkflags", project, configuration);
	libFlags = "/LIBPATH:\"" + libpath + "\" /MACHINE:X64 " + solution->getValue("libflags", project, configuration);

	if (!wrapperObjDir.empty())
		linkFlags += " \"" + FilePath::combine(wrapperObjDir, "libcef_dll_wrapper.lib") + "\"";

	outputExe = project->output;
}

void MsvcTarget::Build()
{
	MsvcCompile::Build(this);
	MsvcRc::Build(this);
	if (FilePath::has_extension(outputExe, "lib"))
		MsvcLib::Build(this);
	else
		MsvcLink::Build(this);
}

void MsvcTarget::Clean()
{
	MsvcCompile::Clean(this);
	MsvcRc::Clean(this);
	if (FilePath::has_extension(outputExe, "lib"))
		MsvcLib::Clean(this);
	else
		MsvcLink::Clean(this);
}

/////////////////////////////////////////////////////////////////////////////

void MsvcCompile::Build(MsvcTarget* target)
{
	for (const std::string& inputFile : target->CppFiles)
	{
		std::string filename = FilePath::last_component(inputFile);

		std::string cppFile = FilePath::combine(target->srcDir, inputFile);
		std::string objFile = FilePath::combine(target->objDir, FilePath::remove_extension(filename) + ".obj");
		std::string depFile = FilePath::combine(target->objDir, FilePath::remove_extension(filename) + ".d");

		bool needsCompile = false;
		try
		{
			int64_t objTime = File::get_last_write_time(objFile);
			for (const std::string& dependency : ReadDependencyFile(depFile))
			{
				int64_t depTime = File::get_last_write_time(dependency);
				if (depTime > objTime)
				{
					needsCompile = true;
					break;
				}
			}
		}
		catch (...)
		{
			needsCompile = true;
		}

		if (needsCompile)
		{
			std::string commandline = target->cl + " " + target->compileFlags + " /nologo /sourceDependencies \"" + depFile + "\" /Fo\"" + objFile + "\" /c " + cppFile;
			int result = std::system(("\"" + commandline + "\"").c_str());
			if (result != 0)
				throw std::runtime_error("Could not compile " + filename);
		}
	}
}

void MsvcCompile::Clean(MsvcTarget* target)
{
	for (const std::string& inputFile : target->CppFiles)
	{
		std::string filename = FilePath::last_component(inputFile);
		std::string objFile = FilePath::combine(target->objDir, FilePath::remove_extension(filename) + ".obj");
		File::try_remove(objFile);
	}
}

std::vector<std::string> MsvcCompile::ReadDependencyFile(const std::string& filename)
{
	std::vector<std::string> files;

	JsonValue depJson = JsonValue::parse(File::read_all_text(filename));

	if (!depJson["Data"]["Source"].is_undefined())
		files.push_back(depJson["Data"]["Source"].to_string());

	if (!depJson["Data"]["PCH"].is_undefined())
		files.push_back(depJson["Data"]["PCH"].to_string());

	for (const JsonValue& includeFile : depJson["Data"]["Includes"].items())
	{
		files.push_back(includeFile.to_string());
	}

	for (const JsonValue& moduleFile : depJson["Data"]["Modules"].items())
	{
		files.push_back(moduleFile.to_string());
	}

	return files;
}

/////////////////////////////////////////////////////////////////////////////

void MsvcRc::Build(MsvcTarget* target)
{
	for (const std::string& inputFile : target->RcFiles)
	{
		std::string filename = FilePath::last_component(inputFile);

		std::string rcFile = FilePath::combine(target->srcDir, inputFile);
		std::string resFile = FilePath::combine(target->objDir, FilePath::remove_extension(filename) + ".res");

		bool needsCompile = false;
		try
		{
			int64_t objTime = File::get_last_write_time(resFile);
			int64_t depTime = File::get_last_write_time(rcFile);
			if (depTime > objTime)
			{
				needsCompile = true;
				break;
			}
		}
		catch (...)
		{
			needsCompile = true;
		}

		if (needsCompile)
		{
			std::string commandline = target->rc + " " + target->rcFlags + " /nologo /fo\"" + resFile + "\" " + rcFile;
			int result = std::system(("\"" + commandline + "\"").c_str());
			if (result != 0)
				throw std::runtime_error("Could not compile " + filename);
		}
	}
}

void MsvcRc::Clean(MsvcTarget* target)
{
	for (const std::string& inputFile : target->RcFiles)
	{
		std::string filename = FilePath::last_component(inputFile);
		std::string objFile = FilePath::combine(target->objDir, FilePath::remove_extension(filename) + ".res");
		File::try_remove(objFile);
	}
}

/////////////////////////////////////////////////////////////////////////////

void MsvcLink::Build(MsvcTarget* target)
{
	std::vector<std::string> objFiles;
	for (const std::string& inputFile : target->CppFiles)
	{
		std::string filename = FilePath::last_component(inputFile);
		std::string objFile = FilePath::combine(target->objDir, FilePath::remove_extension(filename) + ".obj");
		objFiles.push_back(objFile);
	}

	for (const std::string& inputFile : target->RcFiles)
	{
		std::string filename = FilePath::last_component(inputFile);
		std::string objFile = FilePath::combine(target->objDir, FilePath::remove_extension(filename) + ".res");
		objFiles.push_back(objFile);
	}

	bool needsLink = false;
	try
	{
		int64_t exeTime = File::get_last_write_time(FilePath::combine(target->objDir, target->outputExe));
		for (const std::string& dependency : objFiles)
		{
			int64_t depTime = File::get_last_write_time(dependency);
			if (depTime > exeTime)
			{
				needsLink = true;
				break;
			}
		}
		if (!needsLink)
		{
			for (const std::string& manifestFile : target->ManifestFiles)
			{
				int64_t depTime = File::get_last_write_time(FilePath::combine(target->srcDir, manifestFile));
				if (depTime > exeTime)
				{
					needsLink = true;
					break;
				}
			}
		}
	}
	catch (...)
	{
		needsLink = true;
	}

	if (needsLink)
	{
		std::cout << "Linking " << target->outputExe << std::endl;

		std::string responsefilename = FilePath::combine(target->objDir, "responsefile.1");
		std::string responsefile;
		responsefile.reserve(64 * 1024);
		responsefile += target->linkFlags;
		responsefile += " /nologo /out:";
		responsefile += FilePath::combine(target->objDir, target->outputExe);
		responsefile += " /manifest:embed";
		for (const std::string& manifestFile : target->ManifestFiles)
		{
			responsefile += " /manifestinput:\"";
			responsefile += FilePath::combine(target->srcDir, manifestFile);
			responsefile += "\"";
		}
		for (const auto& file : objFiles)
		{
			responsefile.push_back(' ');
			responsefile += file;
		}
		File::write_all_text(responsefilename, responsefile);

		std::string cmdline = target->link + " @\"" + responsefilename + "\"";
		int result = std::system(("\"" + cmdline + "\"").c_str());
		if (result != 0)
			throw std::runtime_error("Could not link " + target->outputExe);

		std::string srcfilename = FilePath::combine(target->objDir, target->outputExe);
		std::string destfilename = FilePath::combine(target->binDir, target->outputExe);
		auto data = File::read_all_bytes(srcfilename);
		File::write_all_bytes(destfilename, data.data(), data.size());
	}
}

void MsvcLink::Clean(MsvcTarget* target)
{
	File::try_remove(target->outputExe);
}

/////////////////////////////////////////////////////////////////////////////

void MsvcLib::Build(MsvcTarget* target)
{
	std::vector<std::string> objFiles;
	for (const std::string& inputFile : target->CppFiles)
	{
		std::string filename = FilePath::last_component(inputFile);
		std::string objFile = FilePath::combine(target->objDir, FilePath::remove_extension(filename) + ".obj");
		objFiles.push_back(objFile);
	}

	bool needsLib = false;
	try
	{
		int64_t exeTime = File::get_last_write_time(FilePath::combine(target->objDir, target->outputExe));
		for (const std::string& dependency : objFiles)
		{
			int64_t depTime = File::get_last_write_time(dependency);
			if (depTime > exeTime)
			{
				needsLib = true;
				break;
			}
		}
	}
	catch (...)
	{
		needsLib = true;
	}

	if (needsLib)
	{
		std::cout << "Creating " << target->outputExe << std::endl;

		std::string responsefilename = FilePath::combine(target->objDir, "responsefile.1");
		std::string responsefile;
		responsefile.reserve(64 * 1024);
		responsefile += target->libFlags;
		responsefile += " /nologo /out:";
		responsefile += FilePath::combine(target->objDir, target->outputExe);
		for (const auto& file : objFiles)
		{
			responsefile.push_back(' ');
			responsefile += file;
		}
		File::write_all_text(responsefilename, responsefile);

		std::string cmdline = target->lib + " @\"" + responsefilename + "\"";
		int result = std::system(("\"" + cmdline + "\"").c_str());
		if (result != 0)
			throw std::runtime_error("Could not create " + target->outputExe);
	}
}

void MsvcLib::Clean(MsvcTarget* target)
{
	File::try_remove(target->outputExe);
}
