// Copyright (c) 2021 Pathfinders
// This file is part of BedrockFormat.
//
// BedrockFormat is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BedrockFormat is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BedrockFormat.  If not, see <https://www.gnu.org/licenses/>.

#ifndef BEDROCKFORMAT_CHUNK_H
#define BEDROCKFORMAT_CHUNK_H

#include "format.h"
#include "nbt.h"

typedef struct Subchunk_T {
    unsigned char version;
    unsigned short blocks[4096]; // 4096 blocks
    unsigned short paletteSize;
    NbtTag** palette;
} Subchunk;

typedef struct SubchunkPosition_T {
    int x;
    unsigned char y;
    int z;
} SubchunkPosition;

Result LoadSubchunk(World* pWorld, Subchunk** ppSubchunk, int x, unsigned char y, int z, Dimension dimension);
void FreeSubchunk(Subchunk* pSubchunk);
void PrintSubchunk(Subchunk* pSubchunk);

NbtTag* GetBlockAtPosition(Subchunk* pSubchunk, unsigned char x, unsigned char y, unsigned char z);

#endif //BEDROCKFORMAT_CHUNK_H
