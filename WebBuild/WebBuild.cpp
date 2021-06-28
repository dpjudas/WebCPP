
#include "Precomp.h"
#include "WebBuild.h"
#include "VSGenerator.h"
#include "File.h"
#include "Environment.h"
#include "EmscriptenTarget.h"
#include "MsvcTarget.h"
#include "CefTarget.h"
#include "Solution.h"

WebBuild::WebBuild(std::string projectname, std::string configuration, std::string solutionDir) : projectname(projectname), configuration(configuration)
{
	solution = Solution::load(FilePath::combine(solutionDir, "websolution.json"));
}

void WebBuild::build()
{
	for (auto& it : solution.projects)
	{
		Project& p = it.second;
		if (p.name == projectname && p.configurations.find(configuration) != p.configurations.end())
		{
			std::string binDir = FilePath::combine(solution.solutionDir, "Build\\" + configuration + "\\bin");
			std::string objDir = FilePath::combine(solution.solutionDir, "Build\\" + configuration + "\\obj\\" + p.name);
			std::string wrapperDir = FilePath::combine(solution.solutionDir, "Build\\" + configuration + "\\obj\\cefwrapper");

			Directory::create(binDir);
			Directory::create(objDir);

			if (p.type == "wasm-static-lib" || p.type == "wasm-webpkg")
			{
				EmscriptenTarget target;
				target.Setup(&solution, &p, configuration, binDir, objDir);
				target.Build();
			}
			else if (p.type == "cef-app")
			{
				Directory::create(wrapperDir);

				CefWrapperLib wrapperlib;
				wrapperlib.Build(&solution, configuration, wrapperDir);

				CefCopyResources copyresources;
				copyresources.Build(binDir);

				MsvcTarget target;
				target.Setup(&solution, &p, configuration, binDir, objDir, wrapperDir);
				target.Build();
			}
		}
	}
}

void WebBuild::clean()
{
	for (auto& it : solution.projects)
	{
		Project& p = it.second;
		if (p.name == projectname && p.configurations.find(configuration) != p.configurations.end())
		{
			std::string binDir = FilePath::combine(solution.solutionDir, "Build\\" + configuration + "\\bin");
			std::string objDir = FilePath::combine(solution.solutionDir, "Build\\" + configuration + "\\obj\\" + p.name);
			std::string wrapperDir = FilePath::combine(solution.solutionDir, "Build\\" + configuration + "\\obj\\cefwrapper");

			if (p.type == "wasm-static-lib" || p.type == "wasm-app")
			{
				EmscriptenTarget target;
				target.Setup(&solution, &p, configuration, binDir, objDir);
				target.Clean();
			}
			else if (p.type == "cef-app")
			{
				CefWrapperLib wrapperlib;
				wrapperlib.Build(&solution, configuration, wrapperDir);

				CefCopyResources copyresources;
				copyresources.Build(binDir);

				MsvcTarget target;
				target.Setup(&solution, &p, configuration, binDir, objDir, wrapperDir);
				target.Clean();
			}
		}
	}
}

void WebBuild::rebuild()
{
	clean();
	build();
}
