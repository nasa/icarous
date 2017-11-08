//
// Created by Swee on 11/7/17.
//

#ifndef CFS_DATA_FORMAT_H
#define CFS_DATA_FORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#ifdef CFS
#include "cfe.h"
#define TLM_HDR_SIZE CFE_SB_TLM_HDR_SIZE
#define CMD_HDR_SIZE CFE_SB_CMD_HDR_SIZE
#else
#define TLM_HDR_SIZE 1
#define CMD_HDR_SIZE 1
#endif


// Icarous Command types
typedef enum {
    _LOOKUP_,
    _COMMAND_,
    _LOOKUP_RETURN_,
    _COMMAND_RETURN_,
} messageType_t;

typedef struct{
    uint8_t TlmHeader[TLM_HDR_SIZE];
    messageType_t mType;
    int id;
    char name[50];
    double argsD[4];
    bool argsB[4];
}PlexilCommandMsg;

#ifdef __cplusplus
}
#endif


#endif //CFS_DATA_FORMAT_H
