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
	if(PFB_FAILED(result)) {
	    return 1;
	}

//	int max_x = 150, max_y = 16, max_z = 150;
//
//    auto t1 = std::chrono::high_resolution_clock::now();
//	for(int x = 0; x < max_x; x++) {
//	    for(int y = 0; y < max_y; y++) {
//	        for(int z = 0; z < max_z; z++) {
//                Subchunk* chunk;
//                result = LoadSubchunk(world, &chunk, x, y, z, OVERWORLD);
//                if(FAILED(result)) {
//                    return 1;
//                }
//
//                FreeSubchunk(chunk);
//	        }
//	    }
//	}
//    auto t2 = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
//
//	std::cout << "Parsed a 16x16x16 subchunk in " << duration << " milliseconds" << std::endl;

    Subchunk* subchunk;
	result = LoadSubchunk(world, &subchunk, 0, 0, 0, OVERWORLD);
	if(PFB_FAILED(result)) {
	    fprintf(stderr, "LoadSubchunk failed with error: %s\n", FormatErrorToString(result));
	    return 1;
	}

	PrintNbtTag(GetBlockAtPosition(subchunk, 0, 0, 0));
	FreeSubchunk(subchunk);

	result = CloseWorld(world);
	if(PFB_FAILED(result)) {
	    return 1;
	}
	return 0;
}