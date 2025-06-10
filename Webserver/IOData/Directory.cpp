
#include "Precomp.h"
#include "Directory.h"
#include "FilePath.h"
#include "Text/UTF16.h"
#include <stdexcept>

#ifndef WIN32
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#else
#define NOMINMAX
#include <Windows.h>
#endif

std::vector<std::string> Directory::files(const std::string& filename)
{
#ifdef WIN32
	std::vector<std::string> files;

	WIN32_FIND_DATAW fileinfo;
	HANDLE handle = FindFirstFileW(to_utf16(filename).c_str(), &fileinfo);
	if (handle == INVALID_HANDLE_VALUE)
		return {};

	try
	{
		do
		{
			bool is_directory = !!(fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
			if (!is_directory)
				files.push_back(from_utf16(fileinfo.cFileName));
		} while (FindNextFileW(handle, &fileinfo) == TRUE);
		FindClose(handle);
	}
	catch (...)
	{
		FindClose(handle);
		throw;
	}

	return files;
#else
	throw std::runtime_error("Directory::files not implemented");
#endif
}

std::vector<std::string> Directory::folders(const std::string& filename)
{
#ifdef WIN32
	std::vector<std::string> files;

	WIN32_FIND_DATAW fileinfo;
	HANDLE handle = FindFirstFileW(to_utf16(filename).c_str(), &fileinfo);
	if (handle == INVALID_HANDLE_VALUE)
		return {};

	try
	{
		do
		{
			bool is_directory = !!(fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
			if (is_directory)
			{
				files.push_back(from_utf16(fileinfo.cFileName));
				if (files.back() == "." || files.back() == "..")
					files.pop_back();
			}
		} while (FindNextFileW(handle, &fileinfo) == TRUE);
		FindClose(handle);
	}
	catch (...)
	{
		FindClose(handle);
		throw;
	}

	return files;
#else
	throw std::runtime_error("Directory::folders not implemented");
#endif
}

#ifdef WIN32
std::string Directory::exePath()
{
	WCHAR exe_filename[1024];
	DWORD len = GetModuleFileNameW(nullptr, exe_filename, 1024);
	if (len == 0 || len == 1024)
		throw std::runtime_error("GetModuleFileName failed!");
	return FilePath::removeLastComponent(from_utf16(exe_filename));
}
#else
std::string Directory::exePath()
{
	char exe_file[PATH_MAX];
#ifdef __APPLE__
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	if (mainBundle)
	{
		CFURLRef mainURL = CFBundleCopyBundleURL(mainBundle);

		if (mainURL)
		{
			int ok = CFURLGetFileSystemRepresentation(
				mainURL, (Boolean)true, (UInt8*)exe_file, PATH_MAX
			);

			if (ok)
			{
				return std::string(exe_file) + "/";
			}
		}
	}

	throw std::runtime_error("get_exe_path failed");

#else
#ifndef PROC_EXE_PATH
#define PROC_EXE_PATH "/proc/self/exe"
#endif
	int size;
	struct stat sb;
	if (lstat(PROC_EXE_PATH, &sb) < 0)
	{
#ifdef EXTERN___PROGNAME
		char* pathenv, * name, * end;
		char fname[PATH_MAX];
		char cwd[PATH_MAX];
		struct stat sba;

		exe_file[0] = '\0';
		if ((pathenv = getenv("PATH")) != nullptr)
		{
			for (name = pathenv; name; name = end)
			{
				if ((end = strchr(name, ':')))
					*end++ = '\0';
				snprintf(fname, sizeof(fname),
					"%s/%s", name, (char*)__progname);
				if (stat(fname, &sba) == 0) {
					snprintf(exe_file, sizeof(exe_file),
						"%s/", name);
					break;
				}
			}
		}
		// if getenv failed or path still not found
		// try current directory as last resort
		if (!exe_file[0])
		{
			if (getcwd(cwd, sizeof(cwd)) != nullptr)
			{
				snprintf(fname, sizeof(fname),
					"%s/%s", cwd, (char*)__progname);
				if (stat(fname, &sba) == 0)
					snprintf(exe_file, sizeof(exe_file),
						"%s/", cwd);
			}
		}
		if (!exe_file[0])
			throw std::runtime_error("get_exe_path: could not find path");
		else
			return std::string(exe_file);
#else
		throw std::runtime_error("get_exe_path: proc file system not accesible");
#endif
	}
	else
	{
		size = readlink(PROC_EXE_PATH, exe_file, PATH_MAX);
		if (size < 0)
		{
			throw std::runtime_error(strerror(errno));
		}
		else
		{
			exe_file[size] = '\0';
			return std::string(dirname(exe_file)) + "/";
		}
	}
#endif

}
#endif

void Directory::create(const std::string& path)
{
#ifdef WIN32
	BOOL result = CreateDirectoryW(to_utf16(path).c_str(), nullptr);
	if (result == FALSE)
	{
		DWORD error = GetLastError();
		if (error == ERROR_ALREADY_EXISTS)
		{
			return;
		}
		else if (error == ERROR_PATH_NOT_FOUND)
		{
			try
			{
				std::string parent = FilePath::removeLastComponent(path);
				if (!parent.empty())
				{
					Directory::create(parent);
					if (CreateDirectoryW(to_utf16(path).c_str(), nullptr) == TRUE)
						return;
				}
			}
			catch (...)
			{
			}
		}
		throw std::runtime_error("Could not create directory for path " + path);
	}
#else
	throw std::runtime_error("Directory::create not implemented");
#endif
}
