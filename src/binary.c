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

#include "binary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Creates a new ByteStream with preallocated memory
ByteStream* CreateByteStream(unsigned int preAllocated) {
    ByteStream* pStream = malloc(sizeof(ByteStream));
    if(pStream == NULL) {
        fprintf(stderr, "Failed to allocate ByteStream\n");
        return NULL;
    }

    pStream->buffer = malloc(preAllocated);
    if(pStream->buffer == NULL) {
        fprintf(stderr, "Failed to allocate ByteStream buffer (size = %i)\n", preAllocated);
        return NULL;
    }

    pStream->position = 0;
    return pStream;
}

// Creates a new ByteStream and fills the inner buffer with the specified data
ByteStream* CreateFilledByteStream(unsigned char* data, unsigned int size) {
    ByteStream* stream = CreateByteStream(size);
    if(stream == NULL) {
        return NULL;
    }

    // Copy the given buffer into the byte stream buffer
    memcpy(stream->buffer, data, size);

    return stream;
}

// Destroys a BitStream and if specified, also frees the inner buffer
void DestroyByteStream(ByteStream* pStream, int freeBuffer) {
    if(freeBuffer) free(pStream->buffer);
    free(pStream);
}

void WriteByte(ByteStream* pStream, unsigned char value) {
    pStream->buffer[pStream->position] = value;
    pStream->position++;
}

void WriteShort(ByteStream* pStream, short value) {
    for(unsigned char i = 0; i < 2; i++) {
        pStream->buffer[pStream->position + i] = (char)value << (i * 8);
    }
    pStream->position += 2;
}

void WriteInt(ByteStream* pStream, int value) {
    for(unsigned char i = 0; i < 4; i++) {
        pStream->buffer[pStream->position + i] = (char)value << (i * 8);
    }
    pStream->position += 4;
}

void WriteLong(ByteStream* pStream, long value) {
    for(unsigned char i = 0; i < 8; i++) {
        pStream->buffer[pStream->position + i] = (char)value << (i * 8);
    }
    pStream->position += 8;
}

void WriteFloat(ByteStream* pStream, float value) {
    union {
        unsigned char bytes[4];
        float f;
    } u;

    u.f = value;

    for(unsigned char i = 0; i < 4; i++) {
        pStream->buffer[pStream->position + i] = u.bytes[i];
    }
    pStream->position += 4;
}

void WriteDouble(ByteStream* pStream, double value) {
    union {
        unsigned char bytes[8];
        double d;
    } u;

    u.d = value;

    for(unsigned char i = 0; i < 8; i++) {
        pStream->buffer[pStream->position + i] = u.bytes[i];
    }
    pStream->position += 8;
}

unsigned char ReadByte(ByteStream* pStream) {
    pStream->position++;
    return pStream->buffer[pStream->position - 1];
}

short ReadShort(ByteStream* pStream) {
    int value = 0;
    for(unsigned char i = 0; i < 2; i++) {
        value += pStream->buffer[pStream->position + i] >> (i * 8);
    }
    pStream->position += 2;
    return (short)value;
}

int ReadInt(ByteStream* pStream) {
    int value = 0;
    for(unsigned char i = 0; i < 4; i++) {
        value += pStream->buffer[pStream->position + i] >> (i * 8);
    }
    pStream->position += 4;
    return value;
}

long ReadLong(ByteStream* pStream) {
    long value = 0;
    for(unsigned char i = 0; i < 8; i++) {
        value += pStream->buffer[pStream->position + i] >> (i * 8);
    }
    pStream->position += 8;
    return value;
}

float ReadFloat(ByteStream* pStream) {
    union {
        unsigned char bytes[4];
        float f;
    } u;

    for(unsigned char i = 0; i < 4; i++) {
        u.bytes[i] = pStream->buffer[pStream->position + i];
    }
    pStream->position += 4;

    return u.f;
}

double ReadDouble(ByteStream* pStream) {
    union {
        unsigned char bytes[8];
        double d;
    } u;

    for(unsigned char i = 0; i < 8; i++) {
        u.bytes[i] = pStream->buffer[pStream->position + i];
    }
    pStream->position += 8;

    return u.d;
}

void Advance(ByteStream* pStream, unsigned int amount) {
    pStream->position += amount;
}