//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#ifndef BEDROCK_FORMAT_NBT_HPP
#define BEDROCK_FORMAT_NBT_HPP

#include "bitstream.hpp"

namespace Pathfinders::Bedrock {
	class NBT {
	public:
		explicit NBT(BitStream& buffer) noexcept;
		~NBT() noexcept = default;

	private:
		BitStream& m_buffer;
	};
}

#endif // BEDROCK_FORMAT_NBT_HPP