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

#ifndef BEDROCK_FORMAT_FORMAT_HPP
#define BEDROCK_FORMAT_FORMAT_HPP

#include "hashmap.h"

#define BF_FAILED(x) x != SUCCESS
#define BF_UNUSED(x) (void)x

typedef enum Result_T {
    SUCCESS,
    SUBCHUNK_NOT_FOUND,
    ALLOCATION_FAILED,
    DATABASE_OPEN_ERROR,
    DATABASE_READ_ERROR,
    INVALID_DATA,
    DESERIALIZATION_FAILED
} Result;

typedef struct World_T {
    void* db;
    struct hashmap_s chunkCache;
} World;

typedef enum Dimension_T {
    OVERWORLD,
    NETHER,
    END
} Dimension;

#ifdef __cplusplus
extern "C" {
#endif

Result OpenWorld(const char *path, World** ppWorld);
Result CloseWorld(World* pWorld);

Result LoadEntry(
    World* pWorld, const unsigned char* key, unsigned int keyLen, unsigned char** ppBuffer, unsigned int* pBufferLen
);

void ClearChunkCache(World* world);
const char* TranslateErrorString(Result error);

#ifdef __cplusplus
}
#endif

#endif //BEDROCK_FORMAT_FORMAT_HPP
