#ifndef BEDROCK_FORMAT_FORMAT_HPP
#define BEDROCK_FORMAT_FORMAT_HPP

#include "subchunk.hpp"
#include "format.hpp"
#include "position.hpp"

#include <map>

typedef std::map<int, std::map<char, std::map<int, Pathfinders::Bedrock::Subchunk>>> SubchunkListXYZ;
typedef std::map<char, std::map<int, Pathfinders::Bedrock::Subchunk>> SubchunkListYZ;
typedef std::map<int, Pathfinders::Bedrock::Subchunk> SubchunkListZ;

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

			Subchunk* GetSubchunk(const SubchunkPosition& position);

		private:
			leveldb::DB* m_database = nullptr;
			SubchunkListXYZ m_subchunks;
	};
}

#endif //BEDROCK_FORMAT_FORMAT_HPP
