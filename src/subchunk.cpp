//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#include "subchunk.hpp"
#include "key.hpp"
#include "format.hpp"
#include "nbt.hpp"

#include <leveldb/db.h>

#include <sstream>
#include <stdexcept>
#include <iostream>

namespace Pathfinders::Bedrock {
	// Create a new empty subchunk to encode
	Subchunk::Subchunk(leveldb::DB* database) : m_database(database) {}

	// Create a new decoded subchunk
	Subchunk::Subchunk(leveldb::DB* database, const SubchunkPosition& position) : m_database(database) {
		leveldb::Slice key = leveldb::Slice(GenerateSubchunkKey(position));

		for(char c : key.ToString()) {
			std::cout << (int)c;
		}
		std::cout << std::endl;

		std::string data;
		leveldb::Status status = m_database->Get(World::ReadOptions, key, &data);

		if(!status.ok()) {
			std::ostringstream ss;
			ss << "Failed to load subchunk data: " << status.ToString();

			throw std::runtime_error(ss.str());
		}

		m_rawData = BitStream(data);
		Decode();
	}

	inline void Subchunk::Decode() {
		m_version = m_rawData.ReadByte();
		unsigned char storageCount = m_version == 1 ? (char)1 : m_rawData.ReadByte();

		for(unsigned char i = 0; i < storageCount; i++) {
			DecodeBlockStorage(i);
		}
	}

	void Subchunk::DecodeBlockStorage(char i) {
		unsigned char version = m_rawData.ReadByte();
		unsigned char bitsPerBlock = version >> 1;
		unsigned char blocksPerWord = floor(32 / bitsPerBlock);

		unsigned int blockStatesSize = ceil(4096 / blocksPerWord);
		m_rawData.Advance(blockStatesSize);

		BlockStorage block = {};
		block.version = version;

		DecodePalette(&block);

		m_storageRecords.push_back(block);
	}

	void Subchunk::DecodePalette(BlockStorage* block) {
		NBT decoder = NBT(m_rawData);
		std::vector<PaletteEntry> palette = decoder.decodePalette();

		block->palette = palette;
	}

	void Subchunk::Encode() {

	}

	void Subchunk::Test() {
		std::cout << m_rawData.GetBuffer().size() << std::endl;
		std::cout << "Version: " << m_version << std::endl;
		std::cout << "Storage records: " << m_storageRecords.size() << std::endl;
	}

	Subchunk::~Subchunk() = default;
}