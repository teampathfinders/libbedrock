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
#include "binary.h"
#include "format.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

inline unsigned char* GenerateSubchunkKey(int x, unsigned char y, int z, Dimension dimension, unsigned int* pBufferLen) {
    unsigned int bufferSize = 4 + 4 + 1 + 1;
    if(dimension != OVERWORLD) bufferSize += 4;

    ByteStream* stream = CreateByteStream(bufferSize);
    if(stream == NULL) {
        return NULL;
    }

    WriteInt(stream, x);
    WriteInt(stream, y);
    if(dimension != OVERWORLD) WriteInt(stream, dimension);

    WriteByte(stream, 0x2f);
    WriteByte(stream, y);

    unsigned char* buffer = stream->buffer;
    *pBufferLen = bufferSize;

    DestroyByteStream(stream, 0);
    return buffer;
}

Result LoadSubchunk(World* pWorld, Subchunk** ppSubchunk, int x, unsigned char y, int z, Dimension dimension) {
    Subchunk* pSubchunk = malloc(sizeof(Subchunk));
    if(pSubchunk == NULL) {
        fprintf(stderr, "Failed to allocate subchunk\n");
        return ALLOCATION_FAILED;
    }

    // Generate the database key that corresponds to the requested subchunk
    unsigned int keyLen;
    unsigned char* key = GenerateSubchunkKey(x, y, z, dimension, &keyLen);
    if(keyLen == 0) {
        return ALLOCATION_FAILED;
    }

    // Load the subchunk from the database using the generated key
    unsigned int rawBufferLen;
    unsigned char* rawBuffer = LoadEntry(pWorld, key, keyLen, &rawBufferLen);
    free(key);
    if(rawBufferLen == 0) {
        return DATABASE_READ_ERROR;
    }

    // Move the buffer into a byte stream to be able to read and write
    ByteStream* stream = CreateFilledByteStream(rawBuffer, rawBufferLen);
    if(stream == NULL) {
        free(rawBuffer);
        return ALLOCATION_FAILED;
    }

    pSubchunk->version = ReadByte(stream);
    printf("Subchunk version: %i\n", pSubchunk->version);
    if(pSubchunk->version != 1 && pSubchunk->version != 8) {
        // Invalid subchunk
        fprintf(
            stderr, "Subchunk %i, %i, %i has version %i (should be either 1 or 8)\n",
            x, y, z, pSubchunk->version
        );
        DestroyByteStream(stream, 1);
        free(pSubchunk);
        return INVALID_DATA;
    }

    if(pSubchunk->version == 1) {
        pSubchunk->storageCount = 1;
    } else {
        pSubchunk->storageCount = ReadByte(stream);
    }

    unsigned char bitsPerBlock = pSubchunk->version >> 1;
    printf("Bits per block: %i\n", bitsPerBlock);

    unsigned char blocksPerWord = floor(32 / bitsPerBlock);
    printf("Blocks per word: %i\n", blocksPerWord);

    unsigned int blockStatesSize = ceil(4096 / blocksPerWord);
    printf("Block states size: %i\n", blockStatesSize);

    *ppSubchunk = pSubchunk;

    // Cleanup
    DestroyByteStream(stream, 1);

    return SUCCESS;
}

Result LoadChunk(World* pWorld, Chunk** ppChunk, int x, int z, Dimension dimension) {


    return SUCCESS;
}