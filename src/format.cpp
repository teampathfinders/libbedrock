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

#include "BedrockFormat/format.h"

extern "C" {
    #include "BedrockFormat/chunk.h"
};

#include <iostream>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
#include <leveldb/env.h>
#include <leveldb/options.h>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>
#include <leveldb/zlib_compressor.h>
#include <leveldb/db.h>
#include <leveldb/decompress_allocator.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

static leveldb::ReadOptions readOptions;
static leveldb::Options options;

/// @brief Opens a new world
/// @param path Path to the world
/// @param world Double pointer to a world struct that will be populated with data
/// @returns Result
Result OpenWorld(const char* path, World** world) {
    *world = new World();
    World* pWorld = *world;

    if(hashmap_create(1, &pWorld->chunkCache) != 0) {
        std::cerr << "Failed to create chunk cache hashmap" << std::endl;
        return ALLOCATION_FAILED;
    }

    // Configuration
    if(options.filter_policy == nullptr) {
        options.filter_policy = leveldb::NewBloomFilterPolicy(10);
        options.block_cache = leveldb::NewLRUCache(40 * 1024 * 1024);
        options.write_buffer_size = 4 * 1024 * 1024;
        options.compressors[0] = new leveldb::ZlibCompressorRaw(-1);
        options.compressors[1] = new leveldb::ZlibCompressor();
    }
    if(readOptions.decompress_allocator == nullptr) {
        readOptions.decompress_allocator = new leveldb::DecompressAllocator();
    }

    pWorld->leveldbCache = options.block_cache;

    // Open database
    leveldb::Status status = leveldb::DB::Open(options, path, (leveldb::DB**)&pWorld->db);
    if(!status.ok()) {
        std::cerr << "Failed to open database with error: " << status.ToString() << std::endl;
        return DATABASE_OPEN_ERROR;
    }

    return SUCCESS;
}

/// @brief Closes the LevelDB database and frees the world
/// @param world World to be freed
/// @returns Result
Result CloseWorld(World* world) {
    ClearChunkCache(world);
    hashmap_destroy(&world->chunkCache);
    delete (leveldb::DB*)world->db;
    delete options.filter_policy;
    delete options.block_cache;
    delete options.compressors[0];
    delete options.compressors[1];
    delete world;

    return SUCCESS;
}

/// @brief Loads a database entry
/// @param world World containing the database
/// @param key Key used to load the entry
/// @param keyLen Length of the key
/// @param buffer Buffer to put the data into
/// @param bufferLen Pointer to an integer containing the length of the data
/// @returns Result
/// @internal
Result LoadEntry(
    World* world, const unsigned char* key, unsigned int keyLen, unsigned char** buffer, unsigned int* bufferLen
) {
    leveldb::Slice slice = leveldb::Slice(reinterpret_cast<const char*>(key), keyLen);
    std::string cppValue;

    // Load from database
    leveldb::Status status = ((leveldb::DB*)world->db)->Get(readOptions, slice, &cppValue);
    if(!status.ok()) {
        if(status.IsNotFound()) {
            return SUBCHUNK_NOT_FOUND;
        }
        return DATABASE_READ_ERROR;
    }

    // Copy the string content into a char array to be able to use it in C
    *bufferLen = cppValue.length();
    auto tempBuffer = new unsigned char[cppValue.length() + 1];

    memcpy(tempBuffer, cppValue.c_str(), cppValue.length());
    *buffer = tempBuffer;

    return SUCCESS;
}

/// @brief Runs for every hashmap entry and frees it
/// @internal
int ClearChunkCacheEntry(void* const context, struct hashmap_element_s* const e) {
    FreeSubchunk((World*)context, (Subchunk*)e->data);
    return -1;
}

/// @brief Clears the chunk cache and frees all the loaded subchunks from memory
/// @param world World containing the chunk cache
void ClearChunkCache(World* world) {
    if(hashmap_iterate_pairs(&world->chunkCache, ClearChunkCacheEntry, world)) {
        std::cerr << "Failed to free all subchunks in chunk cache" << std::endl;
    }
    ((leveldb::Cache*)world->leveldbCache)->Prune();
}

/// @brief Converts a result to a readable string
/// @param error Error to be translated
/// @returns Error string
const char* TranslateErrorString(Result error) {
    switch(error) {
        case SUCCESS:
            return "SUCCESS";
        case SUBCHUNK_NOT_FOUND:
            return "SUBCHUNK_NOT_FOUND";
        case ALLOCATION_FAILED:
            return "ALLOCATION_FAILED";
        case DATABASE_OPEN_ERROR:
            return "DATABASE_OPEN_ERROR";
        case DATABASE_READ_ERROR:
            return "DATABASE_READ_ERROR";
        case INVALID_DATA:
            return "INVALID_DATA";
        case DESERIALIZATION_FAILED:
            return "DESERIALIZATION_FAILED";
        case HASHMAP_INSERTION_FAILED:
            return "HASHMAP_INSERTION_FAILED";
        default:
            return "UNKNOWN";
    }
}