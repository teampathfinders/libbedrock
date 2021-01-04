#include "format.hpp"

#include <leveldb/env.h>
#include <leveldb/options.h>
#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>
#include <leveldb/zlib_compressor.h>
#include <leveldb/decompress_allocator.h>

class EmptyLogger : public leveldb::Logger {
	public:
		void Logv(const char*, va_list) override {}
};

namespace Pathfinders::Bedrock {
	leveldb::ReadOptions World::ReadOptions = leveldb::ReadOptions();

	World::World(const char *path, size_t lruCacheCapacity, size_t writeBufferSize) {
		leveldb::Options options;
		options.filter_policy = leveldb::NewBloomFilterPolicy(10);
		options.block_cache = leveldb::NewLRUCache(lruCacheCapacity);
		options.write_buffer_size = writeBufferSize;
		options.info_log = new EmptyLogger();
		options.compressors[0] = new leveldb::ZlibCompressorRaw(-1);
		options.compressors[1] = new leveldb::ZlibCompressor();

		if(ReadOptions.decompress_allocator == nullptr)
			ReadOptions.decompress_allocator = new leveldb::DecompressAllocator();

		assert(leveldb::DB::Open(options, path, &mDatabase).ok());
	}

	World::~World() {
		delete mDatabase;
	}

	Subchunk& World::GetSubchunk(const SubchunkPosition &position) {
		return mSubchunks.at(position);
	}
}