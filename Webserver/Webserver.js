import { Target, Environment, PackageInstaller, Directory } from "cppbuild";

var includeFiles = [
	"Include/Webserver/ActionModule.h",
	"Include/Webserver/FileModule.h",
	"Include/Webserver/WebContext.h",
	"Include/Webserver/WebController.h",
	"Include/Webserver/WebHeaderName.h",
	"Include/Webserver/WebModule.h",
	"Include/Webserver/WebRequest.h",
	"Include/Webserver/WebRequestUrl.h",
	"Include/Webserver/WebRequestVerb.h",
	"Include/Webserver/WebResponse.h",
	"Include/Webserver/Webserver.h",
	"Include/Webserver/JsonValue.h",
	"Include/Webserver/DataBuffer.h",
];

var files = [
	"Webserver.js",
	"Precomp.cpp",
	"Precomp.h",
	"Text/StringFormat.cpp",
	"Text/StringFormat.h",
	"Text/Text.cpp",
	"Text/Text.h",
	"Text/UTF8Reader.cpp",
	"Text/UTF8Reader.h",
	"Json/JsonValue.cpp",
	"IOData/DataBuffer.cpp",
	"IOData/Directory.cpp",
	"IOData/Directory.h",
	"IOData/File.cpp",
	"IOData/File.h",
	"IOData/FilePath.cpp",
	"IOData/FilePath.h",
	"IOData/IODevice.h",
	"IOData/MemoryDevice.cpp",
	"IOData/MemoryDevice.h",
	"Zip/miniz.c",
	"Zip/miniz.h",
	"Zip/ZipReader.cpp",
	"Zip/ZipReader.h",
	"Zip/ZipWriter.cpp",
	"Zip/ZipWriter.h",
	"Logger/Logger.cpp",
	"Logger/Logger.h",
	"Webserver/ActionModule.cpp",
	"Webserver/FileModule.cpp",
	"Webserver/WebContext.cpp",
	"Webserver/WebController.cpp",
	"Webserver/WebHeaderName.cpp",
	"Webserver/WebRequest.cpp",
	"Webserver/WebRequestUrl.cpp",
	"Webserver/WebResponse.cpp",
	"Webserver/Webserver.cpp",
	"Webserver/Socket/BufferedSocket.cpp",
	"Webserver/Socket/BufferedSocket.h",
	"Webserver/Socket/SocketHandle.cpp",
	"Webserver/Socket/SocketHandle.h",
	"Webserver/Socket/SocketWebserver.cpp",
	"Webserver/Socket/SocketWebserver.h",
];

var win32Files = [
	"Text/UTF16.cpp",
	"Text/UTF16.h",
	"Webserver/HttpSys/HttpSysIoContext.h",
	"Webserver/HttpSys/HttpSysReceiveRequest.cpp",
	"Webserver/HttpSys/HttpSysReceiveRequest.h",
	"Webserver/HttpSys/HttpSysSendResponse.cpp",
	"Webserver/HttpSys/HttpSysSendResponse.h",
	"Webserver/HttpSys/HttpSysWebserver.cpp",
	"Webserver/HttpSys/HttpSysWebserver.h",
];

var webserver = Target.addStaticLibrary("Webserver");
webserver.addFiles(files);
webserver.addFiles(includeFiles);
webserver.addIncludePaths([".", "..", "Include"]);
webserver.addDefines(["MINIZ_NO_STDIO"]);

if (Environment.isWindows()) {
	webserver.addDefines(["WIN32", "_WIN32", "UNICODE", "_UNICODE"]);
	webserver.addFiles(win32Files);
}

var win32Artifacts = [
	{ src: Directory.buildPath("Release/bin/Webserver.lib"), dest: "Release/Webserver.lib" },
	{ src: Directory.buildPath("Release/bin/Webserver.pdb"), dest: "Release/Webserver.pdb" },
	{ src: Directory.buildPath("Debug/bin/Webserver.lib"), dest: "Debug/Webserver.lib" },
	{ src: Directory.buildPath("Debug/bin/Webserver.pdb"), dest: "Debug/Webserver.pdb" },
];

var linuxArtifacts = [
	{ src: Directory.buildPath("Release/bin/libWebserver.a"), dest: "Release/libWebserver.a" },
	{ src: Directory.buildPath("Debug/bin/libWebserver.a"), dest: "Debug/libWebserver.a" },
];

var pkg = PackageInstaller.add("Webserver");
pkg.addIncludePaths(["Include"]);
pkg.addLinkLibraries(["Webserver"]);
pkg.addLibraryPaths(["Debug"], { configuration: "Debug" });
pkg.addLibraryPaths(["Release"], { configuration: "Release" });
if (Environment.isWindows()) {
	pkg.addFiles(win32Artifacts);
}
else {
	pkg.addFiles(linuxArtifacts);
}
pkg.addFiles(includeFiles);
