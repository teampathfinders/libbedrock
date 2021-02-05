// Copyright (c) 2021 Pathfinders
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
// * All advertising materials mentioning features or use of this software must display the following acknowledgement: This product includes software developed by Pathfinders and its contributors.
// * Neither the name of Pathfinders nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "binary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief Creates a new bytestream to be used for reading and writing binary
/// @param preAllocated How big the buffer should initially be
/// @returns Pointer to a bytestream
/// @internal
/// @attention The buffer is allocated using malloc, memory will not be set to NULL
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

/// @brief Creates a new bytestream and fills the buffer with initial data
/// @param data Data to fill the internal buffer with
/// @param size Size of the data argument
/// @returns Pointer to a bytestream
/// @internal
ByteStream* CreateFilledByteStream(unsigned char* data, unsigned int size) {
    ByteStream* stream = CreateByteStream(size);
    if(stream == NULL) {
        return NULL;
    }

    // Copy the given buffer into the byte stream buffer
    memcpy(stream->buffer, data, size);

    return stream;
}

/// @brief Destroys the bytestream and optionally frees the internal buffer
/// @param stream Bytestream to be freed
/// @param freeBuffer Boolean to specify if the internal buffer should be freed too
void DestroyByteStream(ByteStream* pStream, int freeBuffer) {
    if(freeBuffer) free(pStream->buffer);
    free(pStream);
}

/// @brief Writes a byte into the buffer in the bytestream
/// @param stream Bytestream to write value into
/// @param value Value to be put written into the buffer
void WriteByte(ByteStream* pStream, unsigned char value) {
    pStream->buffer[pStream->position] = value;
    pStream->position++;
}

/// @brief Writes a short into the buffer in the bytestream
/// @param stream Bytestream to write value into
/// @param value Value to be put written into the buffer
void WriteShort(ByteStream* pStream, short value) {
    for(unsigned char i = 0; i < 2; i++) {
        pStream->buffer[pStream->position + i] = (char)value << (i * 8);
    }
    pStream->position += 2;
}

/// @brief Writes an int into the buffer in the bytestream
/// @param stream Bytestream to write value into
/// @param value Value to be put written into the buffer
void WriteInt(ByteStream* pStream, int value) {
    for(unsigned char i = 0; i < 4; i++) {
        pStream->buffer[pStream->position + i] = (char)value << (i * 8);
    }
    pStream->position += 4;
}

/// @brief Writes a long into the buffer in the bytestream
/// @param stream Bytestream to write value into
/// @param value Value to be put written into the buffer
void WriteLong(ByteStream* pStream, long value) {
    for(unsigned char i = 0; i < 8; i++) {
        pStream->buffer[pStream->position + i] = (char)value << (i * 8);
    }
    pStream->position += 8;
}

/// @brief Writes a float into the buffer in the bytestream
/// @param stream Bytestream to write value into
/// @param value Value to be put written into the buffer
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

/// @brief Writes a double into the buffer in the bytestream
/// @param stream Bytestream to write value into
/// @param value Value to be put written into the buffer
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

/// @brief Reads a byte from the buffer in the bytestream
/// @param stream Bytestream to read the value from
unsigned char ReadByte(ByteStream* pStream) {
    pStream->position++;
    return pStream->buffer[pStream->position - 1];
}

/// @brief Reads a short from the buffer in the bytestream
/// @param stream Bytestream to read the value from
short ReadShort(ByteStream* pStream) {
    int value = 0;
    for(unsigned char i = 0; i < 2; i++) {
        value += pStream->buffer[pStream->position + i] >> (i * 8);
    }
    pStream->position += 2;
    return (short)value;
}

/// @brief Reads an int from the buffer in the bytestream
/// @param stream Bytestream to read the value from
int ReadInt(ByteStream* pStream) {
    int value = 0;
    for(unsigned char i = 0; i < 4; i++) {
        value += pStream->buffer[pStream->position + i] >> (i * 8);
    }
    pStream->position += 4;
    return value;
}

/// @brief Reads a long from the buffer in the bytestream
/// @param stream Bytestream to read the value from
long ReadLong(ByteStream* pStream) {
    long value = 0;
    for(unsigned char i = 0; i < 8; i++) {
        value += pStream->buffer[pStream->position + i] >> (i * 8);
    }
    pStream->position += 8;
    return value;
}

/// @brief Reads a float from the buffer in the bytestream
/// @param stream Bytestream to read the value from
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

/// @brief Reads a double from the buffer in the bytestream
/// @param stream Bytestream to read the value from
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