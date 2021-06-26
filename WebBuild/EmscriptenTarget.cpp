
#include "Precomp.h"
#include "EmscriptenTarget.h"
#include "File.h"
#include "Environment.h"
#include "ZipWriter.h"
#include "JsonValue.h"
#include "CSSTokenizer.h"
#include "Solution.h"
#include "Project.h"
#include <iostream>
#include <future>

void EmscriptenTarget::Setup(Solution* solution, Project* project, std::string configuration, std::string initbinDir, std::string initobjDir)
{
	emsdk = solution->environment["emsdk"].to_string();
	emcc = emsdk + "/upstream/emscripten/emcc";

	Environment::setVariable("EMSDK", ReplaceAll(emsdk, "\\", "/"));
	Environment::setVariable("EM_CONFIG", emsdk + "\\.emscripten");
	Environment::setVariable("EM_CACHE", emsdk + "/upstream/emscripten\\cache");
	Environment::setVariable("EMSDK_NODE", emsdk + "\\node\\14.15.5_64bit\\bin\\node.exe");
	Environment::setVariable("EMSDK_PYTHON", emsdk + "\\python\\3.9.2-1_64bit\\python.exe");
	Environment::setVariable("JAVA_HOME", emsdk + "\\java\\8.152_64bit");

	InputFiles = project->getFilteredSources({ "cpp", "cc", "ixx" });
	srcDir = project->projectDir;
	binDir = initbinDir;
	objDir = initobjDir;

	wwwRootDir = FilePath::combine(srcDir, project->wwwroot);

	Directory::create(objDir);
	Directory::create(binDir);

	cssfile = FilePath::force_slash(FilePath::combine(srcDir, project->cssfile));
	shellfile = FilePath::force_slash(FilePath::combine(srcDir, project->shellfile));
	preJS = FilePath::force_slash(FilePath::combine(srcDir, project->prejs));

	flags = "--bind -s DISABLE_EXCEPTION_CATCHING=0";
	compileFlags = flags + " " + solution->getValue("cppflags", project, configuration);
	linkFlags = flags + " --pre-js=" + preJS + " --shell-file " + shellfile + " " + solution->getValue("linkflags", project, configuration);

	outputHtml = "index.html";
	outputJS = "index.js";
	outputWasm = "index.wasm";
	outputMap = "index.wasm.map";
	outputCSS = "index.css";
	outputPackage = project->output;
}

void EmscriptenTarget::Build()
{
	EmscriptenCompile::Build(this);
	EmscriptenLink::Build(this);
	EmscriptenCSS::Build(this);
	EmscriptenPackage::Build(this);
}

void EmscriptenTarget::Clean()
{
	EmscriptenCompile::Clean(this);
	EmscriptenLink::Clean(this);
	EmscriptenCSS::Clean(this);
	EmscriptenPackage::Clean(this);
}

std::string EmscriptenTarget::ReplaceAll(const std::string& text, const std::string& search, const std::string& replace)
{
	std::string result;
	size_t pos = 0;
	while (true)
	{
		size_t pos2 = text.find(search, pos);
		if (pos2 == std::string::npos)
		{
			result.append(&text[pos], text.size() - pos);
			return result;
		}
		else
		{
			result.append(&text[pos], pos2 - pos);
			result.append(replace);
			pos = pos2 + search.size();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void EmscriptenCompile::Build(EmscriptenTarget* target)
{
	std::vector<std::future<void>> results;

	int numThreads = std::max((int)std::thread::hardware_concurrency(), 2);
	for (int threadIndex = 0; threadIndex < numThreads; threadIndex++)
	{
		auto threadMain = [=]()
		{
			int i = 0;
			for (const std::string& inputFile : target->InputFiles)
			{
				std::string filename = FilePath::last_component(inputFile);
				if (IsCppFile(filename))
				{
					if (i++ % numThreads != threadIndex)
						continue;

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
						std::cout << filename << std::endl;
						std::string commandline = target->emcc + " " + target->compileFlags + " -MD -c " + cppFile + " -o " + objFile;
						int result = std::system(commandline.c_str());
						if (result != 0)
							throw std::runtime_error("Could not compile " + filename);
					}
				}
			}
		};
		results.push_back(std::async(threadMain));
	}
}

void EmscriptenCompile::Clean(EmscriptenTarget* target)
{
	for (const std::string& inputFile : target->InputFiles)
	{
		std::string filename = FilePath::last_component(inputFile);
		if (IsCppFile(filename))
		{
			std::string objFile = FilePath::combine(target->objDir, FilePath::remove_extension(filename) + ".obj");
			File::try_remove(objFile);
		}
	}
}

bool EmscriptenCompile::IsCppFile(const std::string& filename)
{
	for (const char* ext : { "cpp", "cc", "c" })
	{
		if (FilePath::has_extension(filename, ext))
		{
			return true;
		}
	}
	return false;
}

std::vector<std::string> EmscriptenCompile::ReadDependencyFile(const std::string& filename)
{
	std::vector<MakeToken> tokens = ParseMakefile(File::read_all_text(filename));

	if (tokens.size() < 2 || tokens[0].type != MakeTokenType::string || tokens[1].type != MakeTokenType::colon)
		throw std::runtime_error("Invalid or empty dependency file");

	std::vector<std::string> files;

	for (size_t i = 2; i < tokens.size(); i++)
	{
		if (tokens[i].type == MakeTokenType::newline)
			break;
		else if (tokens[i].type != MakeTokenType::string)
			throw std::runtime_error("Parse error reading dependency target");
		files.push_back(tokens[i].value);
	}

	return files;
}

std::vector<EmscriptenCompile::MakeToken> EmscriptenCompile::ParseMakefile(const std::string& text)
{
	std::vector<MakeToken> tokens;

	size_t pos = 0;
	while (pos < text.size())
	{
		pos = text.find_first_not_of(" \t", pos);
		if (pos == std::string::npos)
			break;

		char c, c2, c3;

		c = text[pos];
		c2 = (pos + 1 < text.size()) ? text[pos + 1] : 0;
		c3 = (pos + 2 < text.size()) ? text[pos + 2] : 0;

		if (c == ':')
		{
			MakeToken token;
			token.type = MakeTokenType::colon;
			tokens.push_back(std::move(token));
			pos++;
		}
		else if (c == '\n')
		{
			MakeToken token;
			token.type = MakeTokenType::newline;
			tokens.push_back(std::move(token));
			pos++;
		}
		else if (c == '\r' && c2 == '\n')
		{
			MakeToken token;
			token.type = MakeTokenType::newline;
			tokens.push_back(std::move(token));
			pos += 2;
		}
		else if (c == '\\' && c2 == '\n')
		{
			pos += 2;
		}
		else if (c == '\\' && c2 == '\r' && c3 == '\n')
		{
			pos += 3;
		}
		else
		{
			MakeToken token;
			token.type = MakeTokenType::string;
			while (pos < text.size())
			{
				c = text[pos];
				c2 = (pos + 1 < text.size()) ? text[pos + 1] : 0;
				c3 = (pos + 2 < text.size()) ? text[pos + 2] : 0;

				if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || (c == ':' && c2 == ' '))
				{
					break;
				}
				else if (c == '\\')
				{
					if (pos + 1 >= text.size())
						throw std::runtime_error("Invalid escape encountered");

					if (c2 == '\\')
					{
						token.value.push_back('\\');
						pos += 2;
					}
					else if (c2 == '\n')
					{
						pos += 2;
						break;
					}
					else if (c2 == '\r' && c3 == '\n')
					{
						pos += 3;
						break;
					}
					else
					{
						token.value.push_back('\\');
						pos++;
					}
				}
				else
				{
					token.value.push_back(text[pos]);
					pos++;
				}
			}

			tokens.push_back(std::move(token));
		}
	}
	return tokens;
}

/////////////////////////////////////////////////////////////////////////////

void EmscriptenLink::Build(EmscriptenTarget* target)
{
	std::vector<std::string> objFiles;
	for (const std::string& inputFile : target->InputFiles)
	{
		std::string filename = FilePath::last_component(inputFile);
		if (IsCppFile(filename))
		{
			std::string objFile = FilePath::combine(target->objDir, FilePath::remove_extension(filename) + ".obj");
			objFiles.push_back(objFile);
		}
	}

	bool needsLink = false;
	try
	{
		int64_t exeTime = File::get_last_write_time(FilePath::combine(target->objDir, target->outputHtml));
		for (const std::string& dependency : objFiles)
		{
			int64_t depTime = File::get_last_write_time(dependency);
			if (depTime > exeTime)
			{
				needsLink = true;
				break;
			}
		}
	}
	catch (...)
	{
		needsLink = true;
	}

	if (needsLink)
	{
		std::cout << "Linking " << target->outputHtml << std::endl;

		std::string responsefilename = FilePath::combine(target->objDir, "responsefile.1");
		std::string responsefile;
		responsefile.reserve(64 * 1024);
		responsefile += target->linkFlags;
		responsefile += " -o ";
		responsefile += FilePath::force_slash(FilePath::combine(target->objDir, target->outputHtml));
		for (const auto& file : objFiles)
		{
			responsefile.push_back(' ');
			responsefile += FilePath::force_slash(file);
		}
		File::write_all_text(responsefilename, responsefile);

		std::string cmdline = target->emcc + " @\"" + responsefilename + "\"";
		int result = std::system(("\"" + cmdline + "\"").c_str());
		if (result != 0)
			throw std::runtime_error("Could not link " + target->outputHtml);
	}
}

void EmscriptenLink::Clean(EmscriptenTarget* target)
{
	File::try_remove(FilePath::combine(target->objDir, target->outputHtml));
	File::try_remove(FilePath::combine(target->objDir, target->outputJS));
	File::try_remove(FilePath::combine(target->objDir, target->outputWasm));
	File::try_remove(FilePath::combine(target->objDir, target->outputMap));
}

bool EmscriptenLink::IsCppFile(const std::string& filename)
{
	for (const char* ext : { "cpp", "cc", "c" })
	{
		if (FilePath::has_extension(filename, ext))
		{
			return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////

void EmscriptenCSS::Build(EmscriptenTarget* target)
{
	std::string filename = FilePath::last_component(target->cssfile);
	std::string cssFile = target->cssfile;
	std::string objFile = FilePath::combine(target->objDir, target->outputCSS);
	std::string depFile = FilePath::combine(target->objDir, FilePath::remove_extension(target->outputCSS) + ".cssdep");

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
		Compile(cssFile, objFile, depFile);
	}

	File::try_remove(target->outputCSS);
}

void EmscriptenCSS::Clean(EmscriptenTarget* target)
{
	File::try_remove(FilePath::combine(target->objDir, target->outputCSS));
}

void EmscriptenCSS::Compile(const std::string& inputFile, const std::string& outputFile, const std::string& depFile)
{
	std::cout << FilePath::last_component(inputFile).c_str() << std::endl;

	std::vector<std::string> includes;
	std::string css = ProcessCSSFile(inputFile, File::read_all_text(inputFile), includes);
	WriteDependencyFile(depFile, inputFile, includes);
	File::write_all_text(outputFile, css);
}

std::string EmscriptenCSS::ProcessCSSFile(const std::string& filename, std::string text, std::vector<std::string>& includes, int level)
{
	auto tokenizer = CSSTokenizer::create(text);

	std::string includeCSS;

	size_t importEnd = 0;
	CSSToken token;
	while (true)
	{
		tokenizer->read(token, true);
		if (token.type == CSSTokenType::null)
			break;

		importEnd = token.offset;
		if (token.type == CSSTokenType::atkeyword && token.value == "import")
		{
			tokenizer->read(token, true);
			if (token.type != CSSTokenType::string)
				throw std::runtime_error(FilePath::force_filesys_slash(filename) + ": error: expected string after @import");

			if (level >= 10)
				throw std::runtime_error(FilePath::force_filesys_slash(filename) + ": error: @import recursion too deep");

			std::string includeFilename = FilePath::combine(FilePath::remove_last_component(filename), token.value);
			std::string includeText;
			try
			{
				includeText = File::read_all_text(includeFilename);
			}
			catch (const std::exception&)
			{
				throw std::runtime_error(FilePath::force_filesys_slash(filename) + ": error: could not include '" + token.value);
			}
			includes.push_back(includeFilename);
			includeCSS += ProcessCSSFile(includeFilename, std::move(includeText), includes, level + 1);

			tokenizer->read(token, true);
			if (token.type != CSSTokenType::semi_colon)
				throw std::runtime_error(FilePath::force_filesys_slash(filename) + ": error: ';' expected after import directive");
		}
		else
		{
			break;
		}
	}

	return includeCSS + text.substr(importEnd);
}

std::vector<std::string> EmscriptenCSS::ReadDependencyFile(const std::string& filename)
{
	std::vector<std::string> files;

	JsonValue depJson = JsonValue::parse(File::read_all_text(filename));

	if (!depJson["Source"].is_undefined())
		files.push_back(depJson["Source"].to_string());

	for (const JsonValue& includeFile : depJson["Includes"].items())
	{
		files.push_back(includeFile.to_string());
	}

	return files;
}

void EmscriptenCSS::WriteDependencyFile(const std::string& filename, const std::string& inputFile, const std::vector<std::string>& includes)
{
	JsonValue depJson = JsonValue::object();
	depJson["Source"] = JsonValue::string(inputFile);
	depJson["Includes"] = JsonValue::array(includes);
	File::write_all_text(filename, depJson.to_json());
}

/////////////////////////////////////////////////////////////////////////////

void EmscriptenPackage::Build(EmscriptenTarget* target)
{
	std::cout << "Building web package" << std::endl;

	auto zip = ZipWriter::create(FilePath::combine(target->binDir, target->outputPackage));
	for (std::string objFile : { target->outputHtml, target->outputJS, target->outputWasm, target->outputCSS })
	{
		std::string filename = FilePath::combine(target->objDir, objFile);
		std::vector<uint8_t> data = File::read_all_bytes(filename);
		zip->add_file(FilePath::last_component(filename), data.data(), data.size());
	}
	AddFolder(zip.get(), target->wwwRootDir, "");

	zip->finalize_archive();
	zip.reset();
}

void EmscriptenPackage::Clean(EmscriptenTarget* target)
{
	File::try_remove(target->outputPackage);
}

void EmscriptenPackage::AddFolder(ZipWriter* zip, std::string srcdir, std::string destdir)
{
	for (const std::string& filename : Directory::files(FilePath::combine(srcdir, "*.*")))
	{
		std::vector<uint8_t> data = File::read_all_bytes(FilePath::combine(srcdir, filename));
		zip->add_file(FilePath::combine(destdir, filename), data.data(), data.size());
	}

	for (const std::string& filename : Directory::folders(FilePath::combine(srcdir, "*.*")))
	{
		AddFolder(zip, FilePath::combine(srcdir, filename), FilePath::combine(destdir, filename));
	}
}
