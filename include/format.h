#ifndef BEDROCK_FORMAT_FORMAT_HPP
#define BEDROCK_FORMAT_FORMAT_HPP

#include "hashmap.h"

#define PFB_FAILED(x) x != SUCCESS

typedef enum Result_T {
    SUCCESS,
    SUBCHUNK_NOT_FOUND,
    ALLOCATION_FAILED,
    DATABASE_OPEN_ERROR,
    DATABASE_READ_ERROR,
    INVALID_DATA,
    DESERIALIZATION_FAILED
} Result;

typedef struct World_T {
    void* db;
    struct hashmap_s chunkCache;
} World;

typedef enum Dimension_T {
    OVERWORLD,
    NETHER,
    END
} Dimension;

#ifdef __cplusplus
extern "C" {
#endif

Result OpenWorld(const char *path, World** ppWorld);
Result CloseWorld(World* pWorld);

Result LoadEntry(
    World* pWorld, const unsigned char* key, unsigned int keyLen, unsigned char** ppBuffer, unsigned int* pBufferLen
);

const char* FormatErrorToString(Result error);

#ifdef __cplusplus
}
#endif

#endif //BEDROCK_FORMAT_FORMAT_HPP
