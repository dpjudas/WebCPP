#pragma once

#include "Solution.h"

#include <string>
#include <vector>

class WebBuild
{
public:
	WebBuild(std::string configuration, std::string solutiondir);

	void build();
	void rebuild();
	void clean();

private:
	std::string configuration;
	Solution solution;
};
