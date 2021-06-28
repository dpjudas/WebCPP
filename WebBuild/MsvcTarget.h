#pragma once

#include <vector>
#include <string>
#include <memory>

class Solution;
class Project;

class MsvcTarget
{
public:
	std::vector<std::string> InputFiles;

	void Setup(Solution* solution, Project* project, std::string configuration, std::string initbinDir, std::string initobjDir, std::string wrapperObjDir);
	void Build();
	void Clean();

	std::string msvcDir;
	std::string cl;
	std::string link;
	std::string lib;

	std::string srcDir;
	std::string objDir;
	std::string binDir;

	std::string compileFlags;
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
	static bool IsCppFile(const std::string& filename);
	static std::vector<std::string> ReadDependencyFile(const std::string& filename);
};

class MsvcLink
{
public:
	static void Build(MsvcTarget* target);
	static void Clean(MsvcTarget* target);

private:
	static bool IsCppFile(const std::string& filename);
};

class MsvcLib
{
public:
	static void Build(MsvcTarget* target);
	static void Clean(MsvcTarget* target);

private:
	static bool IsCppFile(const std::string& filename);
};
