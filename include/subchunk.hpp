//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#ifndef BEDROCK_FORMAT_SUBCHUNK_HPP
#define BEDROCK_FORMAT_SUBCHUNK_HPP

#include "position.hpp"

#include <string>
#include <vector>

namespace leveldb {
	class DB;
}

namespace Pathfinders {
	class BitStream;
}

namespace Pathfinders::Bedrock {
	class Subchunk {
		public:
			Subchunk(leveldb::DB* database, const SubchunkPosition& position);
			~Subchunk();

			void Decode();
			void Encode();

		private:
			struct BlockStorage {

			};

			void DecodeBlockStorage(BitStream& stream, char i);

			leveldb::DB* mDatabase;
			std::string mRawData;
			std::vector<BlockStorage> mStorageRecords;
	};
}

#endif // BEDROCK_FORMAT_SUBCHUNK_HPP
