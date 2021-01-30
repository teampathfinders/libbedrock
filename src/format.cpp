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

#include <iostream>

#include <leveldb/env.h>
#include <leveldb/options.h>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>
#include <leveldb/zlib_compressor.h>
#include <leveldb/db.h>
#include <leveldb/decompress_allocator.h>

static leveldb::ReadOptions readOptions;

class EmptyLogger : public leveldb::Logger {
public:
    void Logv(const char*, va_list) override {};
};

PFBResult PFBOpenWorld(const char* path, PFBWorld** ppWorld) {
    if(readOptions.decompress_allocator == nullptr) {
        readOptions.decompress_allocator = new leveldb::DecompressAllocator();
    }

    *ppWorld = new PFBWorld();
    PFBWorld* pWorld = *ppWorld;

    leveldb::Options options;
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);
    options.block_cache = leveldb::NewLRUCache(40 * 1024 * 1024);
    options.write_buffer_size = 4 * 1024 * 1024;
    options.info_log = new EmptyLogger();
    options.compressors[0] = new leveldb::ZlibCompressorRaw(-1);
    options.compressors[1] = new leveldb::ZlibCompressor();

    leveldb::Status status = leveldb::DB::Open(options, path, (leveldb::DB**)&pWorld->db);
    if(!status.ok()) {
        std::cerr << "Failed to open database with error: " << status.ToString() << std::endl;
        return PFB_DATABASE_OPEN_ERROR;
    }

    return PFB_SUCCESS;
}

PFBResult PFBCloseWorld(PFBWorld* pWorld) {
    delete (leveldb::DB*)pWorld->db;
    delete pWorld;

    return PFB_SUCCESS;
}

PFBResult PFBLoadEntry(PFBWorld* pWorld, const char* key, unsigned int keyLen, char** pBuffer, unsigned int* pBufferLen) {
    leveldb::Slice slice = leveldb::Slice(key, keyLen);
    std::string cppValue;

    leveldb::Status status = ((leveldb::DB*)pWorld->db)->Get(readOptions, slice, &cppValue);
    if(!status.ok()) {
        std::cerr << "Failed to read database entry with error: " << status.ToString() << std::endl;
        return PFB_DATABASE_READ_ERROR;
    }

    *pBufferLen = cppValue.length();
    *pBuffer = new char[cppValue.length()];

    memcpy(*pBuffer, cppValue.data(), cppValue.length());

    return PFB_SUCCESS;
}