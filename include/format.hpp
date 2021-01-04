#ifndef BEDROCK_FORMAT_FORMAT_HPP
#define BEDROCK_FORMAT_FORMAT_HPP

namespace leveldb {
	class DB;
}

namespace Pathfinders::Bedrock {
	class World {
		public:
			explicit World(
					const char* path, size_t lruCacheCapacity = 40 * 1024 * 1024, size_t writeBufferSize = 4 * 1024 * 1024
			);
			~World();

		private:
			leveldb::DB* mDatabase = nullptr;
	};
}

#endif //BEDROCK_FORMAT_FORMAT_HPP
