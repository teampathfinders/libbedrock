//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#ifndef BEDROCK_FORMAT_KEY_HPP
#define BEDROCK_FORMAT_KEY_HPP

#include "position.hpp"

#include <string>

namespace Pathfinders::Bedrock {
	std::string GenerateSubchunkKey(const SubchunkPosition& position);
}

#endif // BEDROCK_FORMAT_KEY_HPP
