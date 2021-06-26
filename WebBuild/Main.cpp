
#include "Precomp.h"
#include "WebBuild.h"
#include <iostream>

int main(int argc, char** argv)
{
	try
	{
		std::vector<std::string> args;
		for (int i = 0; i < argc; i++)
			args.push_back(argv[i]);

		if (args.size() != 4)
		{
			std::cout << "webbuild <command> <configuration> <solutiondir>" << std::endl;
			return 1;
		}

		WebBuild app(args[2], args[3]);

		if (args[1] == "build")
		{
			app.build();
			return 0;
		}
		else if (args[1] == "clean")
		{
			app.clean();
			return 0;
		}
		else if (args[1] == "rebuild")
		{
			app.rebuild();
			return 0;
		}
		else
		{
			std::cout << "Unknown command. Supported commands are: build, clean or rebuild" << std::endl;
			return 1;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 255;
	}
}
