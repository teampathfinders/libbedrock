//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#ifndef BEDROCK_FORMAT_POSITION_HPP
#define BEDROCK_FORMAT_POSITION_HPP

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
	};
}

#endif // BEDROCK_FORMAT_POSITION_HPP
