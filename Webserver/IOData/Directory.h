#pragma once

class Directory
{
public:
	static std::vector<std::string> files(const std::string& filename);
	static std::vector<std::string> folders(const std::string& filename);
	static std::string exePath();
	static void create(const std::string& path);
};
