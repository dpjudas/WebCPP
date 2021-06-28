
#include "file.h"
#include <Windows.h>
#include <stdexcept>

std::wstring to_utf16(const std::string& str)
{
	if (str.empty()) return {};
	int needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
	if (needed == 0)
		throw std::runtime_error("MultiByteToWideChar failed");
	std::wstring result;
	result.resize(needed);
	needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &result[0], (int)result.size());
	if (needed == 0)
		throw std::runtime_error("MultiByteToWideChar failed");
	return result;
}

std::string from_utf16(const std::wstring& str)
{
	if (str.empty()) return {};
	int needed = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0, nullptr, nullptr);
	if (needed == 0)
		throw std::runtime_error("WideCharToMultiByte failed");
	std::string result;
	result.resize(needed);
	needed = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), &result[0], (int)result.size(), nullptr, nullptr);
	if (needed == 0)
		throw std::runtime_error("WideCharToMultiByte failed");
	return result;
}

class FileImpl : public File
{
public:
	FileImpl(HANDLE handle) : handle(handle)
	{
	}

	~FileImpl()
	{
		CloseHandle(handle);
	}

	void write(const void *data, int size) override
	{
		BOOL result = WriteFile(handle, data, size, nullptr, nullptr);
		if (result == FALSE)
			throw std::runtime_error("WriteFile failed");
	}

	void read(void *data, size_t size) override
	{
		DWORD bytesRead = 0;
		BOOL result = ReadFile(handle, data, (DWORD)size, &bytesRead, nullptr);
		if (result == FALSE || bytesRead != size)
			throw std::runtime_error("ReadFile failed");
	}

	int64_t size() override
	{
		LARGE_INTEGER fileSize;
		BOOL result = GetFileSizeEx(handle, &fileSize);
		if (result == FALSE)
			throw std::runtime_error("GetFileSizeEx failed");
		return fileSize.QuadPart;
	}

	void seek(int64_t offset, SeekPoint origin)
	{
		LARGE_INTEGER off, newoff;
		off.QuadPart = offset;
		DWORD moveMethod = FILE_BEGIN;
		if (origin == SeekPoint::current) moveMethod = FILE_CURRENT;
		else if (origin == SeekPoint::end) moveMethod = FILE_END;
		BOOL result = SetFilePointerEx(handle, off, &newoff, moveMethod);
		if (result == FALSE)
			throw std::runtime_error("SetFilePointerEx failed");
	}

	HANDLE handle = INVALID_HANDLE_VALUE;
};

std::shared_ptr<File> File::create_always(const std::string &filename)
{
	HANDLE handle = CreateFile(to_utf16(filename).c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (handle == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Could not create " + filename);

	return std::make_shared<FileImpl>(handle);
}

std::shared_ptr<File> File::open_existing(const std::string &filename)
{
	HANDLE handle = CreateFile(to_utf16(filename).c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (handle == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Could not open " + filename);

	return std::make_shared<FileImpl>(handle);
}

std::string File::read_all_text(const std::string& filename)
{
	auto file = open_existing(filename);
	std::string text;
	text.resize((size_t)file->size());
	if (!text.empty())
		file->read(&text[0], text.size());
	return text;
}

std::vector<uint8_t> File::read_all_bytes(const std::string& filename)
{
	auto file = open_existing(filename);
	std::vector<uint8_t> data;
	data.resize((size_t)file->size());
	if (!data.empty())
		file->read(&data[0], data.size());
	return data;
}

/////////////////////////////////////////////////////////////////////////////

std::vector<std::string> Directory::files(const std::string &path)
{
	std::vector<std::string> files;

	WIN32_FIND_DATA fileinfo;
	HANDLE handle = FindFirstFile(to_utf16(FilePath::combine(path, "*")).c_str(), &fileinfo);
	if (handle == INVALID_HANDLE_VALUE)
		return {};

	try
	{
		do
		{
			bool is_directory = !!(fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
			if (!is_directory)
				files.push_back(from_utf16(fileinfo.cFileName));
		} while (FindNextFile(handle, &fileinfo) == TRUE);
		FindClose(handle);
	}
	catch (...)
	{
		FindClose(handle);
		throw;
	}

	return files;
}

std::vector<std::string> Directory::folders(const std::string& path)
{
	std::vector<std::string> folders;

	WIN32_FIND_DATA fileinfo;
	HANDLE handle = FindFirstFile(to_utf16(FilePath::combine(path, "*")).c_str(), &fileinfo);
	if (handle == INVALID_HANDLE_VALUE)
		return {};

	try
	{
		do
		{
			bool is_directory = !!(fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
			if (is_directory && wcscmp(fileinfo.cFileName, L".") != 0 && wcscmp(fileinfo.cFileName, L"..") != 0)
				folders.push_back(from_utf16(fileinfo.cFileName));
		} while (FindNextFile(handle, &fileinfo) == TRUE);
		FindClose(handle);
	}
	catch (...)
	{
		FindClose(handle);
		throw;
	}

	return folders;
}

#if WIN32
std::string Directory::exe_path()
{
	CHAR exe_filename[_MAX_PATH];
	DWORD len = GetModuleFileNameA(NULL, exe_filename, _MAX_PATH);
	if (len == 0 || len == _MAX_PATH)
		throw std::runtime_error("GetModuleFileName failed!");

	CHAR drive[_MAX_DRIVE], dir[_MAX_DIR];
#ifdef _CRT_INSECURE_DEPRECATE
	_splitpath_s(exe_filename, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);
#else
	_splitpath(exe_filename, drive, dir, NULL, NULL);
#endif

	return std::string(drive) + std::string(dir);
}
#else
std::string Directory::exe_path()
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

/////////////////////////////////////////////////////////////////////////////

bool FilePath::has_extension(const std::string &filename, const char *checkext)
{
	auto fileext = extension(filename);
	return _stricmp(fileext.c_str(), checkext) == 0;
}

std::string FilePath::extension(const std::string &filename)
{
	std::string file = last_component(filename);
	std::string::size_type pos = file.find_last_of('.');
	if (pos == std::string::npos)
		return std::string();

#ifndef WIN32
	// Files beginning with a dot is not a filename extension in Unix.
	// This is different from Windows where it is considered the extension.
	if (path_type == FilePathType::file_system && pos == 0)
		return std::string();
#endif

	return file.substr(pos + 1);

}

std::string FilePath::remove_extension(const std::string &filename)
{
	std::string file = last_component(filename);
	std::string::size_type pos = file.find_last_of('.');
	if (pos == std::string::npos)
		return filename;
	else
		return filename.substr(0, filename.length() - file.length() + pos);
}

std::string FilePath::last_component(const std::string &path)
{
#ifdef WIN32
	auto last_slash = path.find_last_of("/\\");
	if (last_slash != std::string::npos)
		return path.substr(last_slash + 1);
	else
		return path;
#else
	auto last_slash = path.find_last_of('/');
	if (last_slash != std::string::npos)
		return path.substr(last_slash + 1);
	else
		return path;
#endif
}

std::string FilePath::remove_last_component(const std::string &path)
{
#ifdef WIN32
	auto last_slash = path.find_last_of("/\\");
	if (last_slash != std::string::npos)
		return path.substr(0, last_slash + 1);
	else
		return std::string();
#else
	auto last_slash = path.find_last_of('/');
	if (last_slash != std::string::npos)
		return path.substr(0, last_slash + 1);
	else
		return std::string();
#endif
}

std::string FilePath::combine(const std::string &path1, const std::string &path2)
{
#ifdef WIN32
	if (path1.empty())
		return path2;
	else if (path2.empty())
		return path1;
	else if (path2.front() == '/' || path2.front() == '\\')
		return path2;
	else if (path1.back() != '/' && path1.back() != '\\')
		return path1 + "\\" + path2;
	else
		return path1 + path2;
#else
	if (path1.empty())
		return path2;
	else if (path2.empty())
		return path1;
	else if (path2.front() == '/')
		return path2;
	else if (path1.back() != '/')
		return path1 + "/" + path2;
	else
		return path1 + path2;
#endif
}
