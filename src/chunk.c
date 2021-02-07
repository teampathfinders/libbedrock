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

#include "BedrockFormat/chunk.h"
#include "BedrockFormat/binary.h"
#include "BedrockFormat/format.h"
#include "BedrockFormat/nbt.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/// @brief Generates a key for a subchunk database entry
/// @param Position Subchunk position
/// @param bufferLen Pointer to an unsigned integer containing the length of the returned buffer
/// @returns The raw subchunk data
/// @internal
inline unsigned char* GenerateSubchunkKey(int x, unsigned char y, int z, Dimension dimension, unsigned int* bufferLen) {
    unsigned int bufferSize = 4 + 4 + 1 + 1;
    if(dimension != OVERWORLD) bufferSize += 4;

    ByteStream* stream = CreateByteStream(bufferSize);
    if(stream == NULL) {
        return NULL;
    }

    WriteInt(stream, x);
    WriteInt(stream, z);
    if(dimension != OVERWORLD) WriteInt(stream, dimension);

    WriteByte(stream, 0x2f);
    WriteByte(stream, y);

    unsigned char* buffer = stream->buffer;
    *bufferLen = bufferSize;

    DestroyByteStream(stream, 0);
    return buffer;
}

/// @brief Loads a subchunk and stores it in the world's chunk cache
/// @param world World the subchunk is located in
/// @param position Position of the subchunk
/// @returns Result
/// @attention This function has to be called before you can use GetBlockAtWorldPosition or GetBlockAtSubchunkPosition
Result LoadSubchunk(World* world, Subchunk** subchunk, int x, unsigned char y, int z, Dimension dimension) {
    printf("Loading subchunk %i, %i, %i\n", x, y, z);

    Subchunk* decoded = malloc(sizeof(Subchunk));
    if(decoded == NULL) {
        fprintf(stderr, "Failed to allocate subchunk\n");
        return ALLOCATION_FAILED;
    }

    // Set position to NULL to make sure that whenever FreeSubchunk is called before the subchunk is added to the chunk
    // cache FreeSubchunk will still work
    decoded->position = NULL;

    // Generate the database key that corresponds to the requested subchunk
    unsigned int keyLen;
    unsigned char* key = GenerateSubchunkKey(x, y, z, dimension, &keyLen);
    if(keyLen == 0) {
        return ALLOCATION_FAILED;
    }

    // Load the subchunk from the database using the generated key
    unsigned int rawBufferLen;
    unsigned char* rawBuffer;
    Result result = LoadEntry(world, key, keyLen, &rawBuffer, &rawBufferLen);
    free(key);

    if(BF_FAILED(result)) {
        return result;
    }

    // Move the buffer into a byte stream to be able to read and write
    ByteStream* stream = CreateFilledByteStream(rawBuffer, rawBufferLen);
    if(stream == NULL) {
        free(rawBuffer);
        return ALLOCATION_FAILED;
    }

    decoded->version = ReadByte(stream);
    if(decoded->version != 8 && decoded->version != 1) {
        // Invalid subchunk
        fprintf(
                stderr, "Subchunk %i, %i, %i has version %i (should be either 1 or 8)\n",
                x, y, z, decoded->version
        );
        DestroyByteStream(stream, 1);
        free(decoded);
        return INVALID_DATA;
    }

    if(decoded->version == 8) {
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
                decoded->blocks[len] = (unsigned short)b;
                len++;

                w >>= bitsPerBlock;
            }
        }

        decoded->paletteSize = (unsigned short)ReadInt(stream);
        decoded->palette = malloc(sizeof(NbtTag*) * decoded->paletteSize);
        if(decoded->palette == NULL) {
            fprintf(stderr, "Failed to allocate 4096 block states\n");
            DestroyByteStream(stream, 1);
            free(decoded);
            return ALLOCATION_FAILED;
        }

        for(unsigned int j = 0; j < decoded->paletteSize; j++) {
            stream->position += 3; // Skip tag type and name

            struct hashmap_s* compoundEntries = malloc(sizeof(struct hashmap_s));
            if(hashmap_create(2, compoundEntries) != 0) {
                fprintf(stderr, "Failed to create hashmap\n");
                DestroyByteStream(stream, 1);
                free(decoded->palette);
                free(decoded);
                return ALLOCATION_FAILED;
            }

            int decodeResult = DecodeNbtTagWithParent(stream, compoundEntries);
            if(!decodeResult) {
                fprintf(stderr, "Failed to decode NTB entry\n");
                hashmap_destroy(compoundEntries);
                DestroyByteStream(stream, 1);
                free(decoded->palette);
                free(decoded);
                return DESERIALIZATION_FAILED;
            }

            NbtTag* tag = malloc(sizeof(NbtTag));
            if(tag == NULL) {
                fprintf(stderr, "Failed to decode NTB entry\n");
                hashmap_destroy(compoundEntries);
                DestroyByteStream(stream, 1);
                free(decoded->palette);
                free(decoded);
                return ALLOCATION_FAILED;
            }

            tag->type = NBT_COMPOUND;
            tag->payload = compoundEntries;

            decoded->palette[j] = tag;
        }
    }

    *subchunk = decoded;

    Position* subchunkPosition = malloc(sizeof(Position));
    if(subchunkPosition == NULL) {
        fprintf(stderr, "Failed to allocate subchunk position\n");
        FreeSubchunk(NULL, decoded);
        return ALLOCATION_FAILED;
    }

    subchunkPosition->x = x;
    subchunkPosition->y = y;
    subchunkPosition->z = z;
    subchunkPosition->dimension = dimension;
    decoded->position = subchunkPosition;

    if(hashmap_put(&world->chunkCache, (char*)subchunkPosition, sizeof(Position), decoded) != 0) {
        fprintf(stderr, "Failed to insert subchunk into chunk cache\n");
        FreeSubchunk(NULL, decoded);
        return HASHMAP_INSERTION_FAILED;
    }

    // Cleanup
    DestroyByteStream(stream, 1);

    return SUCCESS;
}

/// @brief Frees the subchunk and internal palette entries from memory and removes it from the chunk cache
/// @param world The world that contains the subchunk
/// @param subchunk Subchunk to be freed
/// @attention Pass NULL as the pWorld parameter to free this chunk without removing it from the cache
///            (this feature is only really used internally, but it might be helpful)
void FreeSubchunk(World* world, Subchunk* subchunk) {
    if(world != NULL) {
        hashmap_remove(&world->chunkCache, (char*)subchunk->position, sizeof(Position));
    }

    for(unsigned short i = 0; i < subchunk->paletteSize; i++) {
        FreeNbtTag(subchunk->palette[i]);
    }
    free(subchunk->position);
    free(subchunk->palette);
    free(subchunk);
}

/// @brief Logs the subchunk information to the console
/// @param subchunk Subchunk to be logged
void PrintSubchunk(Subchunk* subchunk) {
    printf("Subchunk version: %i\n", subchunk->version);
    printf("Palette block count: %i\n", subchunk->paletteSize);
    for(unsigned short i = 0; i < subchunk->paletteSize; i++) {
        PrintNbtTag(subchunk->palette[i]);
    }
}

/// @brief Retrieves a block from a subchunk
/// @param subchunk Subchunk containing the desired block
/// @param x X-coordinate of the block
/// @param y Y-coordinate of the block
/// @param z Z-coordinate of the block
/// @returns Pointer to an NBT tag
NbtTag* GetBlockAtSubchunkPosition(Subchunk* subchunk, unsigned char x, unsigned char y, unsigned char z) {
    return subchunk->palette[subchunk->blocks[16 * 16 * x + 16 * z + y]];
}

/// @brief Retrieves a block from a world
/// @param world World containing the block
/// @param position Position of the block
/// @returns Pointer to an NBT tag
/// @attention This function will automatically load the subchunk for you if it has not been loaded before.
///            Otherwise it will be loaded from the chunk cache.
/// @attention This function is very similar to GetBlockAtSubchunkPosition,
///            but instead of loading a block from a subchunk it loads it from a world.
NbtTag* GetBlockAtWorldPosition(World* world, Position* position) {
    Subchunk* subchunk = hashmap_get(&world->chunkCache, (const char*)position, sizeof(Position));
    if(subchunk == NULL) {
        Result parseResult = LoadSubchunk(
            world, &subchunk,
            (int)floor((double)position->x / 16.0),
            (unsigned char)floor((double)position->y / 16.0),
            (int)floor((double)position->z / 16.0),
            position->dimension
        );
        if(BF_FAILED(parseResult)) {
            return NULL;
        }
    }

    return subchunk->palette[subchunk->blocks[16 * 16 * position->x + 16 * position->z + position->y]];
}