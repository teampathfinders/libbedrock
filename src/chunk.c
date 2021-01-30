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

#include "chunk.h"

#include <stdio.h>
#include <stdlib.h>

#include "bitstream.hpp"

inline char* GenerateSubchunkKey(int x, unsigned char y, int z, PFBDimension dimension, unsigned int* pBufferLen) {
    unsigned int bufferSize = 4 + 4 + 1 + 1;
    if(dimension != PFB_OVERWORLD) bufferSize += 4;

    char* buffer = malloc(sizeof(bufferSize));
    if(buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory for database key (buffer length = %i)\n", bufferSize);
        return PFB_ALLOCATION_FAILED;
    }
    *pBufferLen = bufferSize;



    return buffer;
}

PFBResult PFBLoadChunk(PFBWorld* pWorld, PFBChunk* pChunk, int x, int z, PFBDimension dimension) {


    return PFB_SUCCESS;
}