#ifndef BEDROCK_FORMAT_FORMAT_HPP
#define BEDROCK_FORMAT_FORMAT_HPP

#define PFB_FAILED(x) x != PFB_SUCCESS

typedef struct PFBWorld_T {
    void* database;
} PFBWorld;

typedef enum PFBResult_T {
    PFB_SUCCESS,
    PFB_DATABASE_OPEN_ERROR
} PFBResult;

#ifdef __cplusplus
extern "C" {
#endif

PFBResult PFBOpenWorld(const char *path, PFBWorld** ppWorld);
PFBResult PFBCloseWorld(PFBWorld* pWorld);

#ifdef __cplusplus
}
#endif

#endif //BEDROCK_FORMAT_FORMAT_HPP
