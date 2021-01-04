#ifndef BEDROCK_FORMAT_FORMAT_HPP
#define BEDROCK_FORMAT_FORMAT_HPP

#include "subchunk.hpp"
#include "format.hpp"
#include "position.hpp"

#include <leveldb/db.h>
#include <leveldb/options.h>

#include <map>

namespace Pathfinders::Bedrock {
	class World {
		public:
			explicit World(
					const char* path, size_t lruCacheCapacity = 40 * 1024 * 1024, size_t writeBufferSize = 4 * 1024 * 1024
			);
			~World();

			static leveldb::ReadOptions ReadOptions;

			Subchunk& GetSubchunk(const SubchunkPosition& position);

		private:
			leveldb::DB* mDatabase = nullptr;
			std::map<SubchunkPosition, Subchunk> mSubchunks;
	};
}

#endif //BEDROCK_FORMAT_FORMAT_HPP
