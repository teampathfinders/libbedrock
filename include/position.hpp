//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#ifndef BEDROCK_FORMAT_POSITION_HPP
#define BEDROCK_FORMAT_POSITION_HPP

#include <cmath>
#include <unordered_map>

namespace Pathfinders::Bedrock {
	enum class Dimension {
		Overworld,
		Nether,
		End
	};

	struct SubchunkPosition {
		int x, z;
		char y;
		Dimension dimension;

		SubchunkPosition(int x, int y, int z, Dimension dimension = Dimension::Overworld) : x(x), y(y == 0 ? (char)0 : (char)floor(256.0 / (double)y)), z(z), dimension(dimension) {}

		bool operator==(const SubchunkPosition& other) const {
			return (x == other.x) && (y == other.y) && (z == other.z) && (dimension == other.dimension);
		}
	};
}

namespace std {
	template<>
	struct ::std::hash<Pathfinders::Bedrock::SubchunkPosition> {
		size_t operator()(const Pathfinders::Bedrock::SubchunkPosition& p) const {
			return
				std::hash<int>()(p.x) ^
				std::hash<int>()(p.z) ^
				std::hash<char>()(p.y) ^
				std::hash<Pathfinders::Bedrock::Dimension>()(p.dimension);
		}
	};
}

#endif // BEDROCK_FORMAT_POSITION_HPP
