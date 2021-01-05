#include "format.hpp"

#include <leveldb/env.h>
#include <leveldb/options.h>
#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>
#include <leveldb/zlib_compressor.h>
#include <leveldb/decompress_allocator.h>

#include <sstream>
#include <stdexcept>

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

		leveldb::Status status = leveldb::DB::Open(options, path, &m_database);
		if(!status.ok()) {
			std::ostringstream ss;
			ss << "Failed to open database: " << status.ToString();

			throw std::runtime_error(ss.str());
		}
	}

	World::~World() {
		delete m_database;
	}

	Subchunk* World::GetSubchunk(const SubchunkPosition& position) {
		auto iterator = m_subchunks.find(position.x);
		if(iterator != m_subchunks.end()) {
			auto yzMap = iterator->second;
			auto yzIterator = yzMap.find(position.y);

			if(yzIterator != yzMap.end()) {
				auto zMap = yzIterator->second;
				auto zIterator = zMap.find(position.z);

				if(zIterator != zMap.end()) {
					return &zIterator->second;
				}
			}
		}

		// Subchunk was not found in index, load it
		m_subchunks[position.x][position.y][position.z] = Subchunk(m_database, position);
		return (Subchunk*)&m_subchunks[position.x][position.y][position.z];
	}
}