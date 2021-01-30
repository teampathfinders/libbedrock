#ifndef BEDROCK_FORMAT_FORMAT_HPP
#define BEDROCK_FORMAT_FORMAT_HPP

#include "chunk.h"

#define PFB_FAILED(x) x != PFB_SUCCESS

typedef enum PFBResult_T {
    PFB_SUCCESS,
    PFB_DATABASE_OPEN_ERROR,
    PFB_DATABASE_READ_ERROR
} PFBResult;

typedef struct PFBWorld_T {
    void* db;
} PFBWorld;

typedef enum PFBDimension_T {
    PFB_OVERWORLD,
    PFB_NETHER,
    PFB_END
} PFBDimension;

#ifdef __cplusplus
extern "C" {
#endif

PFBResult PFBOpenWorld(const char *path, PFBWorld** ppWorld);
PFBResult PFBCloseWorld(PFBWorld* pWorld);

PFBResult PFBLoadEntry(PFBWorld* pWorld, const char* key, unsigned int keyLen, char* pBuffer, unsigned int* pBufferLen);

#ifdef __cplusplus
}
#endif

#endif //BEDROCK_FORMAT_FORMAT_HPP
