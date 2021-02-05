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

#ifndef BEDROCKFORMAT_BINARY_H
#define BEDROCKFORMAT_BINARY_H

typedef struct ByteStream_T {
    unsigned int position;
    unsigned char* buffer;
} ByteStream;

ByteStream* CreateByteStream(unsigned int preAllocated);
ByteStream* CreateFilledByteStream(unsigned char* data, unsigned int size);
void DestroyByteStream(ByteStream* pStream, int freeBuffer);

void WriteByte(ByteStream* pStream, unsigned char value);
void WriteShort(ByteStream* pStream, short value);
void WriteInt(ByteStream* pStream, int value);
void WriteLong(ByteStream* pStream, long value);
void WriteFloat(ByteStream* pStream, float value);
void WriteDouble(ByteStream* pStream, double value);

unsigned char ReadByte(ByteStream* pStream);
short ReadShort(ByteStream* pStream);
int ReadInt(ByteStream* pStream);
long ReadLong(ByteStream* pStream);
float ReadFloat(ByteStream* pStream);
double ReadDouble(ByteStream* pStream);

#endif // BEDROCKFORMAT_BINARY_H
