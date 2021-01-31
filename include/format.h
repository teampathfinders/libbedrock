#ifndef BEDROCK_FORMAT_FORMAT_HPP
#define BEDROCK_FORMAT_FORMAT_HPP

#define PFB_FAILED(x) x != SUCCESS

typedef enum Result_T {
    SUCCESS,
    ALLOCATION_FAILED,
    DATABASE_OPEN_ERROR,
    DATABASE_READ_ERROR,
    INVALID_DATA
} Result;

typedef struct World_T {
    void* db;
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

unsigned char* LoadEntry(World* pWorld, const unsigned char* key, unsigned int keyLen, unsigned int* pBufferLen);

#ifdef __cplusplus
}
#endif

#endif //BEDROCK_FORMAT_FORMAT_HPP
