//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#ifndef BEDROCK_FORMAT_NBT_HPP
#define BEDROCK_FORMAT_NBT_HPP

#include "bitstream.hpp"
#include "subchunk.hpp"

#include <vector>

namespace Pathfinders::Bedrock {
	enum NBT_Tag {
		TAG_End,
		TAG_Byte,
		TAG_Short,
		TAG_Int,
		TAG_Long,
		TAG_Float,
		TAG_Double,
		TAG_Byte_Array,
		TAG_String,
		TAG_List,
		TAG_Compound,
		TAG_Int_Array,
		TAG_Long_Array
	};

	class NBT {
	public:
		explicit NBT(BitStream& buffer) noexcept;
		~NBT() noexcept = default;

		std::vector<Subchunk::PaletteEntry> DecodePalette();

	private:
		void DecodeCompound();

		BitStream& m_buffer;
	};
}

#endif // BEDROCK_FORMAT_NBT_HPP