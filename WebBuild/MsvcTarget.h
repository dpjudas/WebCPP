#pragma once

#include <vector>
#include <string>
#include <memory>

class Solution;
class Project;

class MsvcTarget
{
public:
	void Setup(Solution* solution, Project* project, std::string configuration, std::string initbinDir, std::string initobjDir, std::string wrapperObjDir);
	void Build();
	void Clean();

	std::vector<std::string> CppFiles;
	std::vector<std::string> RcFiles;
	std::vector<std::string> ManifestFiles;

	std::string msvcDir;
	std::string windowsSdkDir;
	std::string cl;
	std::string link;
	std::string lib;
	std::string rc;

	std::string srcDir;
	std::string objDir;
	std::string binDir;

	std::string compileFlags;
	std::string rcFlags;
	std::string linkFlags;
	std::string libFlags;

	std::string outputExe;
};

class MsvcCompile
{
public:
	static void Build(MsvcTarget* target);
	static void Clean(MsvcTarget* target);

private:
	static std::vector<std::string> ReadDependencyFile(const std::string& filename);
};

class MsvcRc
{
public:
	static void Build(MsvcTarget* target);
	static void Clean(MsvcTarget* target);
};

class MsvcLink
{
public:
	static void Build(MsvcTarget* target);
	static void Clean(MsvcTarget* target);
};

class MsvcLib
{
public:
	static void Build(MsvcTarget* target);
	static void Clean(MsvcTarget* target);
};
