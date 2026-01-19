import { Target } from "cppbuild";

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

var exampleWebsite = Target.addWebsite("ExampleWebsite");
exampleWebsite.setCSSRootFile("Main.css");
exampleWebsite.setHtmlShellFile("Shell.html");
exampleWebsite.addFiles(files);
exampleWebsite.addIncludePaths(includePaths);
exampleWebsite.addLinkLibraries(linkLibraries);
