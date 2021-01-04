//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#include "key.hpp"
#include "bitstream.hpp"

namespace Pathfinders::Bedrock {
	std::string GenerateSubchunkKey(const SubchunkPosition& position) {
		BitStream stream = BitStream();

		stream.WriteLittleInt(position.x); // X coordinate
		stream.WriteLittleInt(position.z); // Z coordinate

		if(position.dimension != Dimension::Overworld)
			stream.WriteLittleInt(static_cast<int>(position.dimension)); // Dimension

		stream.WriteByte(0x2F); // Record type (SubchunkPrefix)

		const auto& buffer = stream.GetBuffer();
		return std::string(buffer.begin(), buffer.end());
	}
}