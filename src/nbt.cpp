//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#include "nbt.hpp"

namespace Pathfinders::Bedrock {
	NBT::NBT(Pathfinders::Bedrock::BitStream& buffer) noexcept : m_buffer(buffer) {}
}