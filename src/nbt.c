// Copyright (c) 2021 Pathfinders
// This file is part of BedrockFormat.
//
// BedrockFormat is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BedrockFormat is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BedrockFormat.  If not, see <https://www.gnu.org/licenses/>.

#include "nbt.h"

#include <stdio.h>

void DecodeNBTCompound(ByteStream* pStream) {
    unsigned char tagType = ReadByte(pStream);
    if(tagType != NBT_COMPOUND) {
        fprintf(stderr, "The top-level NBT tag is not a compound type (it is %i)\n", tagType);
        return;
    }

    printf("woof\n");
}