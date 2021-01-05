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
			struct PaletteEntry {

			};

			Subchunk() = default;
			explicit Subchunk(leveldb::DB* database);
			Subchunk(leveldb::DB* database, const SubchunkPosition& position);
			~Subchunk();

			void Decode();
			void Encode();

			void Test();

		private:
			struct BlockStorage {
				unsigned char version;
				std::vector<PaletteEntry> palette;
			};

			void DecodeBlockStorage(char i);
			void DecodePalette(BlockStorage* block);

			leveldb::DB* m_database;
			BitStream m_rawData;

			// Subchunk data
			char m_version;
			std::vector<BlockStorage> m_storageRecords;
	};
}

#endif // BEDROCK_FORMAT_SUBCHUNK_HPP
