import { Target, Environment } from "cppbuild";

var files = [
	"ExampleWebserver.js",
	"Main.cpp",
];

var includePaths = [
	".",
	"../Webserver/Include",
];

var linkLibraries = [
	"Webserver",
];

var exampleWebserver = Target.addConsole("ExampleWebserver");
exampleWebserver.addFiles(files);
exampleWebserver.addIncludePaths(includePaths);
exampleWebserver.addLinkLibraries(linkLibraries);

if (Environment.isWindows()) {
	exampleWebserver.addDefines(["WIN32", "_WIN32", "UNICODE", "_UNICODE", "WIN32_MEAN_AND_LEAN", "NOMINMAX"]);
}
