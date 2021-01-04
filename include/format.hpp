#ifndef BEDROCK_FORMAT_FORMAT_HPP
#define BEDROCK_FORMAT_FORMAT_HPP

#include "subchunk.hpp"
#include "format.hpp"
#include "position.hpp"

#include <unordered_map>

namespace leveldb {
	class DB;
	class ReadOptions;
}

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
			leveldb::DB* m_database = nullptr;
			std::unordered_map<SubchunkPosition, Subchunk> m_subchunks;
	};
}

#endif //BEDROCK_FORMAT_FORMAT_HPP
