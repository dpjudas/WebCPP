import { Project } from "cppbuild";

Project.setName("WebCPP");

Project.addConfiguration("Debug");
Project.addConfiguration("Release");

Project.addSubdirectory("Webserver");
Project.addSubdirectory("WebCPP");
