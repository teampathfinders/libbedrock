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

#include "nbt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* DecodeRawNbtString(ByteStream* pStream) {
    unsigned short length = ReadShort(pStream);
    if(length == 0) return NULL;

    char* string = calloc(length + 1, 1);
    if(string == NULL) {
        fprintf(stderr, "Failed to allocate buffer for NBT string\n");
        return NULL;
    }

    memcpy(string, pStream->buffer + pStream->position, length);
    pStream->position += length;

    return string;
}

int DecodeNbtTagWithParent(ByteStream* pStream, struct hashmap_s* parent) {
    for(;;) {
        enum NbtTagType type = ReadByte(pStream);
        if(type == NBT_END) return 1;

        NbtTag* tag = malloc(sizeof(NbtTag));
        if(tag == NULL) {
            fprintf(stderr, "Failed to allocate NBT tag\n");
            return 0;
        }

        tag->type = type;
        char* name = DecodeRawNbtString(pStream);

        switch(tag->type) {
            case NBT_BYTE:
                tag->payload = malloc(sizeof(unsigned char));
                if(tag->payload == NULL) {
                    free(tag);
                    fprintf(stderr, "Failed to allocate byte on heap\n");
                    return 0;
                }

                unsigned char byteValue = ReadByte(pStream);
                memcpy(tag->payload, &byteValue, sizeof(unsigned char));
                break;
            case NBT_SHORT:
                tag->payload = malloc(sizeof(short));
                if(tag->payload == NULL) {
                    free(tag);
                    fprintf(stderr, "Failed to allocate short on heap\n");
                    return 0;
                }

                short shortValue = ReadShort(pStream);
                memcpy(tag->payload, &shortValue, sizeof(short));
                break;
            case NBT_INT:
                tag->payload = malloc(sizeof(int));
                if(tag->payload == NULL) {
                    free(tag);
                    fprintf(stderr, "Failed to allocate int on heap\n");
                    return 0;
                }

                int intValue = ReadInt(pStream);
                memcpy(tag->payload, &intValue, sizeof(int));
                break;
            case NBT_LONG:
                tag->payload = malloc(sizeof(long));
                if(tag->payload == NULL) {
                    free(tag);
                    fprintf(stderr, "Failed to allocate long on heap\n");
                    return 0;
                }

                long longValue = ReadLong(pStream);
                memcpy(tag->payload, &longValue, sizeof(long));
                break;
            case NBT_FLOAT:
                tag->payload = malloc(sizeof(float));
                if(tag->payload == NULL) {
                    free(tag);
                    fprintf(stderr, "Failed to allocate float on heap\n");
                    return 0;
                }

                float floatValue = ReadFloat(pStream);
                memcpy(tag->payload, &floatValue, sizeof(float));
                break;
            case NBT_DOUBLE:
                tag->payload = malloc(sizeof(double));
                if(tag->payload == NULL) {
                    free(tag);
                    fprintf(stderr, "Failed to allocate double on heap\n");
                    return 0;
                }

                double doubleValue = ReadDouble(pStream);
                memcpy(tag->payload, &doubleValue, sizeof(double));
                break;
            case NBT_STRING:
                tag->payload = DecodeRawNbtString(pStream);
                break;
            case NBT_COMPOUND:
                struct hashmap_s hashmap;
                if(hashmap_create(1, &hashmap) != 0) {
                    free(tag);
                    fprintf(stderr, "Failed to create hashmap\n");
                    return 0;
                }

                tag->payload = malloc(sizeof(struct hashmap_s));
                if(tag->payload == NULL) {
                    free(tag);
                    hashmap_destroy(&hashmap);
                    fprintf(stderr,"Failed to allocate memory for hashmap\n");
                    return 0;
                }

                memcpy(tag->payload, &hashmap, sizeof(struct hashmap_s));
                if(!DecodeNbtTagWithParent(pStream, tag->payload)) {
                    free(tag);
                    hashmap_destroy(&hashmap);
                    fprintf(stderr,"Failed to allocate memory for hashmap\n");
                    return 0;
                }

                break;
            default:
                fprintf(stderr, "Tag type unimplemented or invalid: %i\n", tag->type);
                break;
        }

        // Skip insertion when END tag was found
        if(name == NULL) name = "compound";
        if(tag != NULL) {
            if(hashmap_put(parent, name, strlen(name), tag) != 0) {
                free(tag);
                fprintf(stderr, "Failed to insert NBT tag into hashmap\n");
                return 0;
            }
        }
    }
}

int FreeHashmapEntries(void* const context, void* const value) {
    FreeNbtTag(value);
    return 1;
}

void FreeNbtTag(NbtTag* tag) {
    switch(tag->type) {
        // These type values are stored in the pointer, they do not have to be freed
        case NBT_COMPOUND:
            hashmap_iterate(tag->payload, FreeHashmapEntries, NULL);
            hashmap_destroy(tag->payload);
            break;
        default:
            free(tag->payload);
            break;
    }

    free(tag);
}

int PrintNbtTagHashmapInner(void* const context, struct hashmap_element_s* const e) {
    NbtTag* tag = e->data;

    PrintNbtTagInner(tag->type, tag->payload, e->key, (int)context);
    return 0;
}

void PrintNbtTagInner(enum NbtTagType type, void* const payload, const char* name, int indentation) {
    for(int i = 0; i < indentation; i++) {
        printf("\t");
    }

    printf("%s(", NbtTagTypeToString(type));
    if(strcmp(name, "") == 0) {
        printf("None");
    } else {
        printf("'%s'", name);
    }
    switch(type) {
        case NBT_COMPOUND:
            printf("): %i entries {\n", hashmap_num_entries(payload));

            indentation++;
            int hashmapResult = hashmap_iterate_pairs(payload, PrintNbtTagHashmapInner, (void*)indentation);
            if(hashmapResult != 0) {
                for(int i = 0; i < indentation; i++) {
                    printf("\t");
                }
                printf("Error loading rest of data... \n}\n");
                return;
            }

            indentation--;
            for(int i = 0; i < indentation; i++) {
                printf("\t");
            }
            printf("}\n");
            break;
        case NBT_BYTE:
            printf("): %i\n", *(unsigned char*)payload);
            break;
        case NBT_SHORT:
            printf("): %i\n", *(short*)payload);
            break;
        case NBT_INT:
            printf("): %i\n", *(int*)payload);
            break;
        case NBT_LONG:
            printf("): %lu\n", *(long*)payload);
            break;
        case NBT_FLOAT:
            printf("): %f", *(float*)payload);
            break;
        case NBT_DOUBLE:
            printf("): %f\n", *(double*)payload);
            break;
        case NBT_STRING:
            printf("): '%s'\n", (char*)payload);
            break;
        default:
            printf("): unknown (the tag type is invalid or it has not been implemented)\n");
            break;
    }
}

void PrintNbtTag(NbtTag* tag) {
    PrintNbtTagInner(tag->type, tag->payload, "", 0);
}

const char* NbtTagTypeToString(enum NbtTagType type) {
    switch(type) {
        case NBT_BYTE:
            return "TAG_Byte";
        case NBT_SHORT:
            return "TAG_Short";
        case NBT_INT:
            return "TAG_Int";
        case NBT_LONG:
            return "TAG_Long";
        case NBT_FLOAT:
            return "TAG_Float";
        case NBT_DOUBLE:
            return "TAG_Double";
        case NBT_BYTE_ARRAY:
            return "TAG_Byte_Array";
        case NBT_STRING:
            return "TAG_String";
        case NBT_LIST:
            return "TAG_List";
        case NBT_COMPOUND:
            return "TAG_Compound";
        case NBT_INT_ARRAY:
            return "TAG_Int_Array";
        case NBT_LONG_ARRAY:
            return "TAG_Long_Array";
        default:
            return "TAG_Invalid";
    }
}