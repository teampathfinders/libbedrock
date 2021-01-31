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

#ifndef BEDROCKFORMAT_BINARY_H
#define BEDROCKFORMAT_BINARY_H

typedef struct ByteStream_T {
    unsigned int position;
    unsigned char* buffer;
} ByteStream;

ByteStream* CreateByteStream(unsigned int preAllocated);
ByteStream* CreateFilledByteStream(unsigned char* data, unsigned int size);
void DestroyByteStream(ByteStream* pStream, _Bool freeBuffer);

void WriteByte(ByteStream* pStream, unsigned char value);
void WriteInt(ByteStream* pStream, int value);

unsigned char ReadByte(ByteStream* pStream);
int ReadInt(ByteStream* pStream);

#endif //BEDROCKFORMAT_BINARY_H
