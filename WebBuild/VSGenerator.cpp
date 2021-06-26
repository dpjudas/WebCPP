
#include "Precomp.h"
#include "VSGenerator.h"
#include "File.h"

VSSolution::VSSolution(const std::string& name) : name(name)
{
}

void VSSolution::generate(std::string solutionDir)
{
	VSGenerator generator(solutionDir);
	generator.writeSolution(this);
	for (const auto& project : projects)
	{
		generator.writeProject(project.get());
	}
}

/////////////////////////////////////////////////////////////////////////////

void VSGenerator::writeSolution(const VSSolution* solution)
{
	VSLineWriter output;
	output.writeLine("Microsoft Visual Studio Solution File, Format Version 12.00");
	output.writeLine("# Visual Studio Version 16");
	output.writeLine("VisualStudioVersion = " + solution->visualStudioVersion);
	output.writeLine("MinimumVisualStudioVersion = " + solution->minimumVisualStudioVersion);
	output.writeLine("Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"CPPBuild\", \"CPPBuild\\CPPBuild.vcxproj\", \"{CE286EC1-21B8-40F1-996A-6229F1DD75B1}\"");
	output.writeLine("EndProject");
	for (const auto& project : solution->projects)
	{
		output.writeLine("Project(\"{" + project->typeGuid + "}\") = \"" + project->name + "\", \"" + getProjectFilename(project->name, ".vcxproj") + "\", \"{" + project->projectGuid + "}\"");
		output.writeLine("EndProject");
	}
	output.writeLine("Global");
	output.writeLine("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution");
	for (const auto& configuration : solution->configurations)
	{
		output.writeLine("\t\t" + configuration->name + "|" + configuration->platform + " = " + configuration->name + "|" + configuration->platform);
	}
	output.writeLine("\tEndGlobalSection");
	output.writeLine("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution");
	output.writeLine("\t\t{CE286EC1-21B8-40F1-996A-6229F1DD75B1}.Debug|x64.ActiveCfg = Debug|x64");
	output.writeLine("\t\t{CE286EC1-21B8-40F1-996A-6229F1DD75B1}.Debug|x64.Build.0 = Debug|x64");
	output.writeLine("\t\t{CE286EC1-21B8-40F1-996A-6229F1DD75B1}.Release|x64.ActiveCfg = Release|x64");
	output.writeLine("\t\t{CE286EC1-21B8-40F1-996A-6229F1DD75B1}.Release|x64.Build.0 = Release|x64");
	for (const auto& project : solution->projects)
	{
		for (const auto& configuration : project->configurations)
		{
			output.writeLine("\t\t{" + project->projectGuid + "}." + configuration->name + "|" + configuration->platform + ".ActiveCfg = " + configuration->name + "|" + configuration->platform);
			output.writeLine("\t\t{" + project->projectGuid + "}." + configuration->name + "|" + configuration->platform + ".Build.0 = " + configuration->name + "|" + configuration->platform);
		}
	}
	output.writeLine("\tEndGlobalSection");
	output.writeLine("\tGlobalSection(SolutionProperties) = preSolution");
	output.writeLine("\t\tHideSolutionNode = FALSE");
	output.writeLine("\tEndGlobalSection");
	output.writeLine("\tGlobalSection(ExtensibilityGlobals) = postSolution");
	output.writeLine("\t\tSolutionGuid = {" + solution->solutionGuid + "}");
	output.writeLine("\tEndGlobalSection");
	output.writeLine("EndGlobal");
	output.save(FilePath::combine(solutionDir, solution->name + ".sln"));
}

void VSGenerator::writeProject(const VSCppProject* project)
{
	VSLineWriter output;
	output.writeLine("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	output.writeLine("<Project DefaultTargets=\"Build\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">");
	output.writeLine("  <ItemGroup Label=\"ProjectConfigurations\">");
	for (const auto& configuration : project->configurations)
	{
		output.writeLine("    <ProjectConfiguration Include=\"" + configuration->name + "|" + configuration->platform + "\">");
		output.writeLine("      <Configuration>" + configuration->name + "</Configuration>");
		output.writeLine("      <Platform>" + configuration->platform + "</Platform>");
		output.writeLine("    </ProjectConfiguration>");
	}
	output.writeLine("  </ItemGroup>");
	output.writeLine("  <PropertyGroup Label=\"Globals\">");
	output.writeLine("    <VCProjectVersion>" + project->vcProjectVersion + "</VCProjectVersion>");
	output.writeLine("    <Keyword>Win32Proj</Keyword>");
	output.writeLine("    <ProjectGuid>{" + toLowerCase(project->projectGuid) + "}</ProjectGuid>");
	output.writeLine("    <RootNamespace>" + project->name + "</RootNamespace>");
	output.writeLine("    <WindowsTargetPlatformVersion>" + project->windowsTargetPlatformVersion + "</WindowsTargetPlatformVersion>");
	output.writeLine("  </PropertyGroup>");
	output.writeLine("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />");
	for (const auto& configuration : project->configurations)
	{
		output.writeLine("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\" Label=\"Configuration\">");
		output.writeLine("    <ConfigurationType>" + configuration->general.configurationType + "</ConfigurationType>");
		output.writeLine("    <UseDebugLibraries>" + configuration->general.useDebugLibraries + "</UseDebugLibraries>");
		output.writeLine("    <PlatformToolset>" + configuration->general.platformToolset + "</PlatformToolset>");
		if (configuration->general.configurationType != "Makefile")
		{
			output.writeLine("    <WholeProgramOptimization>" + configuration->general.wholeProgramOptimization + "</WholeProgramOptimization>");
			output.writeLine("    <CharacterSet>" + configuration->general.characterSet + "</CharacterSet>");
		}
		output.writeLine("  </PropertyGroup>");
	}
	output.writeLine("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />");
	output.writeLine("  <ImportGroup Label=\"ExtensionSettings\">");
	output.writeLine("  </ImportGroup>");
	output.writeLine("  <ImportGroup Label=\"Shared\">");
	output.writeLine("  </ImportGroup>");
	for (const auto& configuration : project->configurations)
	{
		output.writeLine("  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\">");
		output.writeLine("    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />");
		output.writeLine("  </ImportGroup>");
	}
	output.writeLine("  <PropertyGroup Label=\"UserMacros\" />");
	for (const auto& configuration : project->configurations)
	{
		output.writeLine("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\">");
		if (configuration->general.configurationType != "Makefile")
		{
			output.writeLine("    <LinkIncremental>" + configuration->general.linkIncremental + "</LinkIncremental>");
		}
		else
		{
			std::string nmakePreprocessorDefinitions;
			for (const auto& value : configuration->general.nmakePreprocessorDefinitions)
			{
				if (!nmakePreprocessorDefinitions.empty())
					nmakePreprocessorDefinitions += ";";
				nmakePreprocessorDefinitions += value;
			}
			if (!nmakePreprocessorDefinitions.empty())
			{
				nmakePreprocessorDefinitions += ";";
				nmakePreprocessorDefinitions += "$(NMakePreprocessorDefinitions)";
			}

			std::string nmakeIncludeSearchPath;
			for (const auto& value : configuration->general.nmakeIncludeSearchPath)
			{
				if (!nmakeIncludeSearchPath.empty())
					nmakeIncludeSearchPath += ";";
				nmakeIncludeSearchPath += value;
			}
			if (!nmakeIncludeSearchPath.empty())
			{
				nmakeIncludeSearchPath += ";";
				nmakeIncludeSearchPath += "$(NMakeIncludeSearchPath)";
			}

			output.writeLine("    <NMakeBuildCommandLine>" + configuration->general.nmakeBuildCommandLine + "</NMakeBuildCommandLine>");
			output.writeLine("    <NMakeOutput>" + configuration->general.nmakeOutput + "</NMakeOutput>");
			output.writeLine("    <NMakeCleanCommandLine>" + configuration->general.nmakeCleanCommandLine + "</NMakeCleanCommandLine>");
			output.writeLine("    <NMakeReBuildCommandLine>" + configuration->general.nmakeReBuildCommandLine + "</NMakeReBuildCommandLine>");
			output.writeLine("    <NMakePreprocessorDefinitions>" + nmakePreprocessorDefinitions + "</NMakePreprocessorDefinitions>");
			output.writeLine("    <NMakeIncludeSearchPath>" + nmakeIncludeSearchPath + "</NMakeIncludeSearchPath>");
		}
		output.writeLine("    <OutDir>" + configuration->general.outDir + "</OutDir>");
		output.writeLine("    <IntDir>" + configuration->general.intDir + "</IntDir>");
		output.writeLine("  </PropertyGroup>");
	}
	for (const auto& configuration : project->configurations)
	{
		std::string preprocessorDefinitions;
		for (const auto& value : configuration->clCompile.preprocessorDefinitions)
		{
			if (!preprocessorDefinitions.empty())
				preprocessorDefinitions += ";";
			preprocessorDefinitions += value;
		}
		if (!preprocessorDefinitions.empty())
		{
			preprocessorDefinitions += ";";
			preprocessorDefinitions += "%(PreprocessorDefinitions)";
		}

		std::string additionalIncludeDirectories;
		for (const auto& value : configuration->clCompile.additionalIncludeDirectories)
		{
			if (!additionalIncludeDirectories.empty())
				additionalIncludeDirectories += ";";
			additionalIncludeDirectories += value;
		}
		if (!additionalIncludeDirectories.empty())
		{
			additionalIncludeDirectories += ";";
			additionalIncludeDirectories += "%(AdditionalIncludeDirectories)";
		}

		output.writeLine("  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='" + configuration->name + "|" + configuration->platform + "'\">");
		if (configuration->general.configurationType != "Makefile")
		{
			output.writeLine("    <ClCompile>");
			output.writeLine("      <WarningLevel>" + configuration->clCompile.warningLevel + "</WarningLevel>");
			output.writeLine("      <FunctionLevelLinking>" + configuration->clCompile.functionLevelLinking + "</FunctionLevelLinking>");
			output.writeLine("      <IntrinsicFunctions>" + configuration->clCompile.intrinsicFunctions + "</IntrinsicFunctions>");
			output.writeLine("      <SDLCheck>" + configuration->clCompile.sdlCheck + "</SDLCheck>");
			output.writeLine("      <PreprocessorDefinitions>" + preprocessorDefinitions + "</PreprocessorDefinitions>");
			output.writeLine("      <ConformanceMode>" + configuration->clCompile.conformanceMode + "</ConformanceMode>");
			output.writeLine("      <AdditionalIncludeDirectories>" + additionalIncludeDirectories + "</AdditionalIncludeDirectories>");
			output.writeLine("      <RuntimeLibrary>" + configuration->clCompile.runtimeLibrary + "</RuntimeLibrary>");
			output.writeLine("    </ClCompile>");
			output.writeLine("    <Link>");
			output.writeLine("      <SubSystem>" + configuration->link.subSystem + "</SubSystem>");
			output.writeLine("      <GenerateDebugInformation>" + configuration->link.generateDebugInformation + "</GenerateDebugInformation>");
			output.writeLine("    </Link>");
		}
		output.writeLine("  </ItemDefinitionGroup>");
	}
	output.writeLine("  <ItemGroup>");
	for (const auto& file : project->sourceFiles)
	{
		output.writeLine("    <ClCompile Include=\"" + file + "\" />");
	}
	output.writeLine("  </ItemGroup>");
	output.writeLine("  <ItemGroup>");
	for (const auto& file : project->headerFiles)
	{
		output.writeLine("    <ClInclude Include=\"" + file + "\" />");
	}
	output.writeLine("  </ItemGroup>");
	output.writeLine("  <ItemGroup>");
	for (const auto& file : project->extraFiles)
	{
		output.writeLine("    <None Include=\"" + file + "\" />");
	}
	output.writeLine("  </ItemGroup>");
	output.writeLine("  <ItemGroup>");
	output.writeLine("    <ProjectReference Include=\"..\\..\\CPPBuild\\CPPBuild.vcxproj\">");
	output.writeLine("      <Project>{ce286ec1-21b8-40f1-996a-6229f1dd75b1}</Project>");
	output.writeLine("    </ProjectReference>");
	output.writeLine("  </ItemGroup>");
	output.writeLine("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />");
	output.writeLine("  <ImportGroup Label=\"ExtensionTargets\">");
	output.writeLine("  </ImportGroup>");
	output.writeLine("</Project>");
	output.save(FilePath::combine(solutionDir, getProjectFilename(project->name, ".vcxproj")));

	if (!project->filters.empty())
	{
		writeProjectFilters(project);
	}
}

void VSGenerator::writeProjectFilters(const VSCppProject* project)
{
	VSLineWriter output;
	output.writeLine("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	output.writeLine("<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">");
	output.writeLine("  <ItemGroup>");
	for (const auto& filter : project->filters)
	{
		if (filter->name != "")
		{
			output.writeLine("    <Filter Include=\"" + filter->name + "\">");
			if (!filter->guid.empty())
				output.writeLine("      <UniqueIdentifier>{" + filter->guid + "}</UniqueIdentifier>");
			output.writeLine("    </Filter>");
		}
	}
	output.writeLine("  </ItemGroup>");
	output.writeLine("  <ItemGroup>");
	for (const auto& filter : project->filters)
	{
		for (const auto& file : filter->sourceFiles)
		{
			if (filter->name == "")
			{
				output.writeLine("    <ClCompile Include=\"" + file + "\" />");
			}
			else
			{
				output.writeLine("    <ClCompile Include=\"" + file + "\">");
				output.writeLine("      <Filter>" + filter->name + "</Filter>");
				output.writeLine("    </ClCompile>");
			}
		}
	}
	output.writeLine("  </ItemGroup>");
	output.writeLine("  <ItemGroup>");
	for (const auto& filter : project->filters)
	{
		for (const auto& file : filter->headerFiles)
		{
			if (filter->name == "")
			{
				output.writeLine("    <ClInclude Include=\"" + file + "\" />");
			}
			else
			{
				output.writeLine("    <ClInclude Include=\"" + file + "\">");
				output.writeLine("      <Filter>" + filter->name + "</Filter>");
				output.writeLine("    </ClInclude>");
			}
		}
	}
	output.writeLine("  </ItemGroup>");
	output.writeLine("  <ItemGroup>");
	for (const auto& filter : project->filters)
	{
		for (const auto& file : filter->extraFiles)
		{
			if (filter->name == "")
			{
				output.writeLine("    <None Include=\"" + file + "\" />");
			}
			else
			{
				output.writeLine("    <None Include=\"" + file + "\">");
				output.writeLine("      <Filter>" + filter->name + "</Filter>");
				output.writeLine("    </None>");
			}
		}
	}
	output.writeLine("  </ItemGroup>");
	output.writeLine("</Project>");

	output.save(FilePath::combine(solutionDir, getProjectFilename(project->name, ".vcxproj.filters")));
}

std::string VSGenerator::getProjectFilename(std::string name, std::string extension)
{
	return FilePath::combine(FilePath::combine("Sources", name), name + extension);
}

std::string VSGenerator::toLowerCase(std::string text)
{
	for (char& c : text)
	{
		if (c >= 'A' && c <= 'Z')
			c = c - 'A' + 'a';
	}
	return text;
}

/////////////////////////////////////////////////////////////////////////////

void VSLineWriter::writeLine(const std::string& text)
{
	lines += text;
	lines += "\r\n";
}

void VSLineWriter::save(const std::string& filename)
{
	try
	{
		std::string lastVersion = File::read_all_text(filename);
		if (lastVersion == lines)
			return;
	}
	catch (...)
	{
	}
	File::write_all_text(filename, lines);
}
