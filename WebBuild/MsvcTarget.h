#pragma once

#include <vector>
#include <string>
#include <memory>

class MsvcTarget
{
public:
	std::vector<std::string> InputFiles;

	void Setup(std::string configuration, std::string outputFilename, std::string sourceDir, std::string solutionDir);
	void Build();
	void Clean();

	std::string msvcDir;
	std::string cl;
	std::string link;
	std::string lib;

	std::string srcDir;
	std::string objDir;
	std::string binDir;

	std::string includePath;

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
