
#include "Precomp.h"
#include "WebBuild.h"
#include "VSGenerator.h"
#include "File.h"
#include "Environment.h"
#include "EmscriptenTarget.h"
#include "MsvcTarget.h"
#include "CefTarget.h"
#include "Solution.h"

WebBuild::WebBuild(std::string configuration, std::string solutionDir) : configuration(configuration)
{
	solution = Solution::load(FilePath::combine(solutionDir, "websolution.json"));
}

void WebBuild::build()
{
	for (auto& it : solution.projects)
	{
		Project& p = it.second;
		if (p.configurations.find(configuration) != p.configurations.end())
		{
			std::string binDir = FilePath::combine(solution.solutionDir, "Build\\" + configuration + "\\bin");
			std::string objDir = FilePath::combine(solution.solutionDir, "Build\\" + configuration + "\\obj\\" + p.name);

			EmscriptenTarget target;
			target.Setup(&solution, &p, configuration, binDir, objDir);
			target.Build();
		}
	}
}

void WebBuild::clean()
{
	for (auto& it : solution.projects)
	{
		Project& p = it.second;
		if (p.configurations.find(configuration) != p.configurations.end())
		{
			std::string binDir = FilePath::combine(solution.solutionDir, "Build\\" + configuration + "\\bin");
			std::string objDir = FilePath::combine(solution.solutionDir, "Build\\" + configuration + "\\obj\\" + p.name);

			EmscriptenTarget target;
			target.Setup(&solution, &p, configuration, binDir, objDir);
			target.Clean();
		}
	}
}

void WebBuild::rebuild()
{
	clean();
	build();
}
