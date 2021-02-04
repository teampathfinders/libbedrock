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

#include "format.h"

extern "C" {
    #include "chunk.h"
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

/// @brief An empty logger that is used for the LevelDB database
/// @internal
class EmptyLogger : public leveldb::Logger {
public:
    void Logv(const char*, va_list) override {};
};

/// @brief Opens a new world
/// @param path Path to the world
/// @param world Double pointer to a world struct that will be populated with data
/// @returns Result
Result OpenWorld(const char* path, World** ppWorld) {
    if(readOptions.decompress_allocator == nullptr) {
        readOptions.decompress_allocator = new leveldb::DecompressAllocator();
    }

    *ppWorld = new World();
    World* pWorld = *ppWorld;

    if(hashmap_create(1, &pWorld->chunkCache) != 0) {
        std::cerr << "Failed to create chunk cache hashmap" << std::endl;
        return ALLOCATION_FAILED;
    }

    // Configuration
    leveldb::Options options;
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);
    options.block_cache = leveldb::NewLRUCache(40 * 1024 * 1024);
    options.write_buffer_size = 4 * 1024 * 1024;
    options.info_log = new EmptyLogger();
    options.compressors[0] = new leveldb::ZlibCompressorRaw(-1);
    options.compressors[1] = new leveldb::ZlibCompressor();

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
Result CloseWorld(World* pWorld) {
    hashmap_destroy(&pWorld->chunkCache);
    delete (leveldb::DB*)pWorld->db;
    delete pWorld;

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
    World* pWorld, const unsigned char* key, unsigned int keyLen, unsigned char** ppBuffer, unsigned int* pBufferLen
) {
    leveldb::Slice slice = leveldb::Slice(reinterpret_cast<const char*>(key), keyLen);
    std::string cppValue;

    // Load from database
    leveldb::Status status = ((leveldb::DB*)pWorld->db)->Get(readOptions, slice, &cppValue);
    if(!status.ok()) {
        if(status.IsNotFound()) {
            return SUBCHUNK_NOT_FOUND;
        }
        return DATABASE_READ_ERROR;
    }

    // Copy the string content into a char array to be able to use it in C
    *pBufferLen = cppValue.length();
    auto buffer = new unsigned char[cppValue.length() + 1];

    memcpy(buffer, cppValue.c_str(), cppValue.length());
    *ppBuffer = buffer;

    return SUCCESS;
}

/// @brief Runs for every hashmap entry and frees it
/// @internal
int ClearChunkCacheEntry(void* const context, struct hashmap_element_s* const e) {
    UNUSED(context);

    FreeSubchunk((Subchunk*)e->data);
    return -1;
}

/// @brief Clears the chunk cache and frees all the loaded subchunks from memory
/// @param world World containing the chunk cache
void ClearChunkCache(World* world) {
    if(hashmap_iterate_pairs(&world->chunkCache, ClearChunkCacheEntry, nullptr)) {
        std::cerr << "Failed to free all subchunks in chunk cache" << std::endl;
    }
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
        default:
            return "UNKNOWN";
    }
}