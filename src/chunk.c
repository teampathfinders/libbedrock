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
#include "nbt.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/// @brief Generates a key for a subchunk database entry
/// @param Position Subchunk position
/// @param pBufferLen Pointer to an unsigned integer containing the length of the returned buffer
/// @returns The raw subchunk data
/// @internal
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

/// @brief Loads a subchunk and stores it in the world's chunk cache
/// @param world World the subchunk is located in
/// @param position Position of the subchunk
/// @returns Result
/// @attention This function has to be called before you can use GetBlockAtWorldPosition or GetBlockAtSubchunkPosition
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
    unsigned char* rawBuffer;
    Result result = LoadEntry(pWorld, key, keyLen, &rawBuffer, &rawBufferLen);
    free(key);

    if(PFB_FAILED(result)) {
        return result;
    }

    // Move the buffer into a byte stream to be able to read and write
    ByteStream* stream = CreateFilledByteStream(rawBuffer, rawBufferLen);
    if(stream == NULL) {
        free(rawBuffer);
        return ALLOCATION_FAILED;
    }

    pSubchunk->version = ReadByte(stream);
    if(pSubchunk->version != 8 && pSubchunk->version != 1) {
        // Invalid subchunk
        fprintf(
            stderr, "Subchunk %i, %i, %i has version %i (should be either 1 or 8)\n",
            x, y, z, pSubchunk->version
        );
        DestroyByteStream(stream, 1);
        free(pSubchunk);
        return INVALID_DATA;
    }

    if(pSubchunk->version == 8) {
        stream->position++;
    }

    for(unsigned char i = 0; i < 1; i++) {
        unsigned char version = ReadByte(stream);
        unsigned char bitsPerBlock = version >> 1;

        unsigned int len = 0;
        while(len < 4096) {
            int w = ReadInt(stream);

            unsigned int blockCount = 32 / bitsPerBlock;
            unsigned int allOnes = 0xFFFFFFFF;
            unsigned int lowerZeros = allOnes << bitsPerBlock;
            unsigned int lowerOnes = ~lowerZeros;

            for(unsigned int j = 0; j < blockCount && len < 4096; j++) {
                unsigned int b = w & lowerOnes;
                pSubchunk->blocks[len] = (unsigned short)b;
                len++;

                w >>= bitsPerBlock;
            }
        }

        unsigned int paletteSize = ReadInt(stream);
        pSubchunk->paletteSize = paletteSize;
        pSubchunk->palette = malloc(sizeof(NbtTag*) * paletteSize);
        if(pSubchunk->palette == NULL) {
            fprintf(stderr, "Failed to allocate 4096 block states\n");
            DestroyByteStream(stream, 1);
            free(pSubchunk);
            return ALLOCATION_FAILED;
        }

        for(unsigned int j = 0; j < paletteSize; j++) {
            stream->position += 3; // Skip tag type and name

            struct hashmap_s* compoundEntries = malloc(sizeof(struct hashmap_s));
            if(hashmap_create(2, compoundEntries) != 0) {
                fprintf(stderr, "Failed to create hashmap\n");
                DestroyByteStream(stream, 1);
                free(pSubchunk->palette);
                free(pSubchunk);
                return ALLOCATION_FAILED;
            }

            int decodeResult = DecodeNbtTagWithParent(stream, compoundEntries);
            if(!decodeResult) {
                fprintf(stderr, "Failed to decode NTB entry\n");
                hashmap_destroy(compoundEntries);
                DestroyByteStream(stream, 1);
                free(pSubchunk->palette);
                free(pSubchunk);
                return DESERIALIZATION_FAILED;
            }

            NbtTag* tag = malloc(sizeof(NbtTag));
            if(tag == NULL) {
                fprintf(stderr, "Failed to decode NTB entry\n");
                hashmap_destroy(compoundEntries);
                DestroyByteStream(stream, 1);
                free(pSubchunk->palette);
                free(pSubchunk);
                return ALLOCATION_FAILED;
            }

            tag->type = NBT_COMPOUND;
            tag->payload = compoundEntries;

            pSubchunk->palette[j] = tag;
        }
    }

    *ppSubchunk = pSubchunk;

    // Cleanup
    DestroyByteStream(stream, 1);

    return SUCCESS;
}

/// @brief Frees the subchunk and internal palette entries from memory
/// @param subchunk Subchunk to be freed
void FreeSubchunk(Subchunk* pSubchunk) {
    for(unsigned short i = 0; i < pSubchunk->paletteSize; i++) {
        FreeNbtTag(pSubchunk->palette[i]);
    }
    free(pSubchunk->palette);
    free(pSubchunk);
}

/// @brief Logs the subchunk information to the console
/// @param subchunk Subchunk to be logged
void PrintSubchunk(Subchunk* pSubchunk) {
    printf("Subchunk version: %i\n", pSubchunk->version);
    printf("Palette block count: %i\n", pSubchunk->paletteSize);
    for(unsigned short i = 0; i < pSubchunk->paletteSize; i++) {
        PrintNbtTag(pSubchunk->palette[i]);
    }
}

/// @brief Retrieves a block from a subchunk
/// @param subchunk Subchunk containing the desired block
/// @param x X-coordinate of the block
/// @param y Y-coordinate of the block
/// @param z Z-coordinate of the block
/// @returns Pointer to an NBT tag
NbtTag* GetBlockAtPosition(Subchunk* pSubchunk, unsigned char x, unsigned char y, unsigned char z) {
    return pSubchunk->palette[pSubchunk->blocks[16 * 16 * x + 16 * z + y]];
}

/// @brief Retrieves a block from a world
/// @param world World containing the block
/// @param position Position of the block
/// @returns Pointer to an NBT tag
/// @attention The subchunk the block is located in must have been loaded before using LoadSubchunk
/// @attention This function is very similar to GetBlockAtSubchunkPosition,
///            but instead of loading a block from a subchunk it loads it from a world
NbtTag* GetBlockAtWorldPosition(World* world, Position* position) {
    Subchunk* subchunk = hashmap_get(&world->chunkCache, (const char*)position, sizeof(Position));
    if(subchunk == NULL) {
        return NULL;
    }

    return subchunk->palette[subchunk->blocks[16 * 16 * position->x + 16 * position->z + position->y]];
}