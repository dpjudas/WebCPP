import { Targets } from "cppbuild";

var files = [
	"ExampleWebsite.js",
	"Main.cpp",
	"Main.h",
	"Main.css",
	"Fonts.css",
	"Shell.html",
];

var includePaths = [
	".",
	"../WebCPP/Include",
];

var linkLibraries = [
	"WebCPP",
];

var exampleWebsite = Targets.addWebsite("ExampleWebsite");
exampleWebsite.setCSSRootFile("Main.css");
exampleWebsite.addFiles(files);
exampleWebsite.addIncludePaths(includePaths);
exampleWebsite.addLinkLibraries(linkLibraries);
