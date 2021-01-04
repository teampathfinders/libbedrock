//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#include "subchunk.hpp"
#include "key.hpp"
#include "format.hpp"

#include <leveldb/db.h>

#include <sstream>
#include <stdexcept>

namespace Pathfinders::Bedrock {
	// Create a new empty subchunk to encode
	Subchunk::Subchunk(leveldb::DB* database) : m_database(database) {}

	// Create a new decoded subchunk
	Subchunk::Subchunk(leveldb::DB* database, const SubchunkPosition& position) : m_database(database) {
		leveldb::Slice key = leveldb::Slice(GenerateSubchunkKey(position));
		leveldb::Status status = m_database->Get(World::ReadOptions, key, &m_rawData);
		if(!status.ok()) {
			std::ostringstream ss;
			ss << "Failed to load subchunk data: " << status.ToString();

			throw std::runtime_error(ss.str());
		}

		Decode();
	}

	inline void Subchunk::Decode() {
		BitStream stream = BitStream(m_rawData.data(), m_rawData.size());

		// TODO: Add support for version 1
		m_version = stream.ReadByte();
		char storageCount = m_version == 1 ? (char)0x01 : stream.ReadByte();

		for(char i = 0; i < storageCount; i++) {
			DecodeBlockStorage(stream, i);
		}
	}

	void Subchunk::DecodeBlockStorage(BitStream& stream, char i) {
		char version = stream.ReadByte();
		char bitsPerBlock = version >> 1;
		char blocksPerWord = (char)floor(32 / bitsPerBlock);
		size_t indiceSize = ceil(4096 / blocksPerWord);

		stream.Advance(indiceSize);

		auto paletteSize = (unsigned int)stream.ReadLittleInt();

		m_storageRecords.push_back({ version, blocksPerWord, indiceSize, paletteSize });
	}

	void Subchunk::Encode() {

	}

	Subchunk::~Subchunk() = default;
}