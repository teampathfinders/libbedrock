//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#include "nbt.hpp"

#include <cassert>

namespace Pathfinders::Bedrock {
	NBT::NBT(Pathfinders::Bedrock::BitStream& buffer) noexcept : m_buffer(buffer) {}



	void NBT::DecodeCompound() {
		assert(m_buffer.ReadByte() == TAG_Compound);


	}

	std::vector<Subchunk::PaletteEntry> NBT::DecodePalette() {
		unsigned int paletteSize = m_buffer.ReadLittleInt();

		DecodeCompound();
	}
}