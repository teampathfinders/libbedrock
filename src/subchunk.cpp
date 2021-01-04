//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#include "subchunk.hpp"

namespace Pathfinders::Bedrock {
	Subchunk::Subchunk(leveldb::DB* database, int x, int y, int z, Dimension dimension) : mDatabase(database) {

	}

	Subchunk::~Subchunk() = default;
}