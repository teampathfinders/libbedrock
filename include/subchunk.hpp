//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#ifndef BEDROCK_FORMAT_SUBCHUNK_HPP
#define BEDROCK_FORMAT_SUBCHUNK_HPP

#include "position.hpp"
#include "bitstream.hpp"

#include <string>
#include <vector>

namespace leveldb {
	class DB;
}

namespace Pathfinders::Bedrock {
	class Subchunk {
		public:
			Subchunk() = default;
			explicit Subchunk(leveldb::DB* database);
			Subchunk(leveldb::DB* database, const SubchunkPosition& position);
			~Subchunk();

			void Decode();
			void Encode();

		private:
			struct BlockStorage {
				char version;
				char blocksPerWord;
				unsigned int indiceSize;
				unsigned int paletteSize;
			};

			void DecodeBlockStorage(BitStream& stream, char i);

			leveldb::DB* m_database;
			std::string m_rawData;

			// Subchunk data
			char m_version;
			std::vector<BlockStorage> m_storageRecords;
	};
}

#endif // BEDROCK_FORMAT_SUBCHUNK_HPP
