//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#ifndef BEDROCK_FORMAT_SUBCHUNK_HPP
#define BEDROCK_FORMAT_SUBCHUNK_HPP

#include "dimension.hpp"

namespace leveldb {
	class DB;
}

namespace Pathfinders::Bedrock {
	class Subchunk {
		public:
			Subchunk(leveldb::DB* database, int x, int y, int z, Dimension dimension = Dimension::Overworld);
			~Subchunk();

		private:
			leveldb::DB* mDatabase;
	};
}

#endif // BEDROCK_FORMAT_SUBCHUNK_HPP
