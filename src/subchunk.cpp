//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#include "subchunk.hpp"
#include "key.hpp"
#include "format.hpp"

#include <sstream>
#include <stdexcept>

namespace Pathfinders::Bedrock {
	Subchunk::Subchunk(leveldb::DB* database, const SubchunkPosition& position) : mDatabase(database) {
		leveldb::Slice key = leveldb::Slice(GenerateSubchunkKey(position));
		leveldb::Status status = mDatabase->Get(World::ReadOptions, key, &mRawData);
		if(!status.ok()) {
			std::ostringstream ss;
			ss << "Failed to load subchunk data: " << status.ToString();

			throw std::runtime_error(ss.str());
		}
	}

	void Subchunk::Decode() {
		BitStream stream = BitStream(mRawData.data(), mRawData.size());

		// TODO: Add support for version 1
		char version = stream.ReadByte();
		char storageCount = version == 1 ? (char)0x01 : stream.ReadByte();

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
	}

	void Subchunk::Encode() {

	}

	Subchunk::~Subchunk() = default;
}