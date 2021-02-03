//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

extern "C" {
    #include "format.h"
    #include "chunk.h"
}

#include <windows.h>
#include <shlobj.h>

#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <chrono>

std::string GetSaveDirectory() {
	TCHAR szPath[MAX_PATH];
	if(PFB_FAILED(SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, szPath))) {
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

	World* world;
	Result result = OpenWorld(path.c_str(), &world);
	if(FAILED(result)) {
	    return 1;
	}

    auto t1 = std::chrono::high_resolution_clock::now();
	Subchunk* chunk;
	result = LoadSubchunk(world, &chunk, 0, 0, 0, OVERWORLD);
	if(FAILED(result)) {
	    return 1;
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

	std::cout << "Parsed a 16x16x16 subchunk in " << duration << " microseconds" << std::endl;

	result = CloseWorld(world);
	if(FAILED(result)) {
	    return 1;
	}
	return 0;
}