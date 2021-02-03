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

static int log_hashmap(void* const context, struct hashmap_element_s* const e) {
    printf("\t- %s\n", e->key);
    return 1;
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
//    printf("Subchunk version: %i\n", pSubchunk->version);
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

    unsigned char storageCount;
    if(pSubchunk->version == 1) {
        storageCount = 1;
    } else {
        storageCount = ReadByte(stream);
    }

    pSubchunk->blocks = malloc(sizeof(unsigned short) * 4096);
    if(pSubchunk->blocks == NULL) {
        fprintf(stderr, "Failed to allocate 4096 block states\n");
        DestroyByteStream(stream, 1);
        free(pSubchunk);
        return ALLOCATION_FAILED;
    }

    for(unsigned char i = 0; i < 1; i++) {
        unsigned char version = ReadByte(stream);
        unsigned char bitsPerBlock = version >> 1;
        unsigned char blocksPerWord = (unsigned char)floor(32.0 / (double)bitsPerBlock);
        unsigned int blockDataSize = (unsigned int)ceil(4096.0 / (double)blocksPerWord);

//        printf("--------------------------\nChunk version: %i\n", version);
//        printf("Bits per block: %i\n", bitsPerBlock);
//        printf("Blocks per word: %i\n", blocksPerWord);
//        printf("Block data size: %i bytes\n", blockDataSize);

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
//        printf("Palette size: %i\n", paletteSize);

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
                return ALLOCATION_FAILED;
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

void FreeSubchunk(Subchunk* pSubchunk) {
    for(unsigned short i = 0; i < pSubchunk->paletteSize; i++) {
        FreeNbtTag(pSubchunk->palette[i]);
    }
    free(pSubchunk->palette);
    free(pSubchunk);
}

Result LoadChunk(World* pWorld, Chunk** ppChunk, int x, int z, Dimension dimension) {


    return SUCCESS;
}