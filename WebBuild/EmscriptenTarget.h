#pragma once

#include <vector>
#include <string>
#include <memory>

class Solution;
class Project;
class ZipWriter;

class EmscriptenTarget
{
public:
	void Setup(Solution* solution, Project* project, std::string configuration, std::string binDir, std::string objDir);
	void Build();
	void Clean();

	std::string type;
	std::vector<std::string> InputFiles;

	std::string emsdk;
	std::string emcc;
	std::string emar;

	std::string srcDir;
	std::string objDir;
	std::string binDir;
	std::string wwwRootDir;

	std::string cssfile;

	std::string flags;
	std::string compileFlags;
	std::string linkFlags;

	std::string outputLib;
	std::string outputHtml;
	std::string outputJS;
	std::string outputWasm;
	std::string outputMap;
	std::string outputCSS;
	std::string outputPackage;

private:
	static std::string ReplaceAll(const std::string& text, const std::string& search, const std::string& replace);
};

class EmscriptenCompile
{
public:
	static void Build(EmscriptenTarget* target);
	static void Clean(EmscriptenTarget* target);

private:
	static bool IsCppFile(const std::string& filename);

	enum class MakeTokenType
	{
		string,
		colon,
		newline
	};

	struct MakeToken
	{
		MakeTokenType type = MakeTokenType::string;
		std::string value;
	};

	static std::vector<MakeToken> ParseMakefile(const std::string& text);
	static std::vector<std::string> ReadDependencyFile(const std::string& filename);
};

class EmscriptenLink
{
public:
	static void Build(EmscriptenTarget* target);
	static void Clean(EmscriptenTarget* target);

private:
	static bool IsCppFile(const std::string& filename);
};

class EmscriptenLib
{
public:
	static void Build(EmscriptenTarget* target);
	static void Clean(EmscriptenTarget* target);

private:
	static bool IsCppFile(const std::string& filename);
};

class EmscriptenCSS
{
public:
	static void Build(EmscriptenTarget* target);
	static void Clean(EmscriptenTarget* target);

private:
	static void Compile(const std::string& inputFile, const std::string& outputFile, const std::string& depFile);
	static std::string ProcessCSSFile(const std::string& filename, std::string text, std::vector<std::string>& includes, int level = 0);
	static std::vector<std::string> ReadDependencyFile(const std::string& filename);
	static void WriteDependencyFile(const std::string& filename, const std::string& inputFile, const std::vector<std::string>& includes);
};

class EmscriptenPackage
{
public:
	static void Build(EmscriptenTarget* target);
	static void Clean(EmscriptenTarget* target);

private:
	static void AddFolder(ZipWriter* zip, std::string srcdir, std::string destdir);
};
