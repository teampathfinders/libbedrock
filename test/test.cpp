//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#include "format.h"

#include <windows.h>
#include <shlobj.h>

#include <iostream>
#include <filesystem>
#include <string>
#include <stdexcept>

std::string GetSaveDirectory() {
	TCHAR szPath[MAX_PATH];
	if(FAILED(SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, szPath))) {
		throw std::runtime_error("Failed to get AppData path");
	}

	return std::string(szPath) + R"(\Packages\Microsoft.MinecraftUWP_8wekyb3d8bbwe\LocalState\games\com.mojang\minecraftWorlds)";
}

std::string GetFirstSave() {
	std::string worlds = GetSaveDirectory();

	for(const auto& dir : std::filesystem::directory_iterator(worlds)) {
		if(dir.is_directory()) return (dir.path().string() + "\\db");
	}

	throw std::runtime_error("No worlds found");
}

int main() {
	std::string path = GetFirstSave();
	std::cout << path << std::endl;

	PFBWorld* world;
	PFBResult result = PFBOpenWorld(path.c_str(), &world);
	if(PFB_FAILED(result)) {
	    return 1;
	}

	result = PFBCloseWorld(world);
	if(PFB_FAILED(result)) {
	    return 1;
	}
	return 0;
}