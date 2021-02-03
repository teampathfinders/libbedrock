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

#ifndef BEDROCKFORMAT_NBT_H
#define BEDROCKFORMAT_NBT_H

#include "binary.h"
#include "hashmap.h"

enum NbtTagType {
    NBT_END,
    NBT_BYTE,
    NBT_SHORT,
    NBT_INT,
    NBT_LONG,
    NBT_FLOAT,
    NBT_DOUBLE,
    NBT_BYTE_ARRAY,
    NBT_STRING,
    NBT_LIST,
    NBT_COMPOUND,
    NBT_INT_ARRAY,
    NBT_LONG_ARRAY
};

const char* NbtTagTypeToString(enum NbtTagType type);

typedef struct NbtTag_T {
    enum NbtTagType type;
    void* payload;
} NbtTag;

char* DecodeRawNbtString(ByteStream* pStream);
int DecodeNbtTagWithParent(ByteStream* pStream, struct hashmap_s* parent);

void PrintNbtTagInner(enum NbtTagType type, void* payload, const char* name, int indentation);
void PrintNbtTag(NbtTag* tag);
void FreeNbtTag(NbtTag* tag);

#endif //BEDROCKFORMAT_NBT_H
