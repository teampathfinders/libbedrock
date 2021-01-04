//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#include "key.hpp"
#include "bitstream.hpp"

namespace Pathfinders::Bedrock {
	std::string GenerateSubchunkKey(int x, int y, int z, Dimension dimension) {
		BitStream stream = BitStream();

		stream.WriteLittleInt(x); // X coordinate
		stream.WriteLittleInt(z); // Z coordinate
		if(dimension != Dimension::Overworld) stream.WriteLittleInt(static_cast<int>(dimension)); // Dimension
		stream.WriteByte(0x2F); // Record type (SubchunkPrefix)

		const auto& buffer = stream.GetBuffer();
		return std::string(buffer.begin(), buffer.end());
	}
}