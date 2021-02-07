// Copyright (c) 2021 Pathfinders
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
// * All advertising materials mentioning features or use of this software must display the following acknowledgement: This product includes software developed by Pathfinders and its contributors.
// * Neither the name of Pathfinders nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

extern "C" {
    #include "BedrockFormat/format.h"
    #include "BedrockFormat/chunk.h"
}

#include <windows.h>
#include <shlobj.h>

#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <chrono>
#include <thread>

std::string GetSaveDirectory() {
	TCHAR szPath[MAX_PATH];
	if(BF_FAILED(SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, szPath))) {
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
	if(BF_FAILED(result)) {
	    return 1;
	}

	Position position = { 0, 6, 0, OVERWORLD };
	NbtTag* block = GetBlockAtWorldPosition(world, &position);

	if(block != NULL) {
        PrintNbtTag(block);
	} else {
	    std::cout << "NULL" << std::endl;
	}

//	int max_x = 100, max_y = 16, max_z = 100;
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
//	        }
//	    }
//	}
//    auto t2 = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
//
//	std::cout << "Parsed a 16x16x16 subchunk in " << duration << " milliseconds" << std::endl;

//    Subchunk* subchunk;
//	result = LoadSubchunk(world, &subchunk, 0, 0, 0, OVERWORLD);
//	if(BF_FAILED(result)) {
//	    fprintf(stderr, "LoadSubchunk failed with error: %s\n", TranslateErrorString(result));
//	    return 1;
//	}
//
//	PrintSubchunk(subchunk);
//	PrintNbtTag(GetBlockAtPosition(subchunk, 0, 0, 0));

	result = CloseWorld(world);
	if(BF_FAILED(result)) {
	    return 1;
	}
	return 0;
}