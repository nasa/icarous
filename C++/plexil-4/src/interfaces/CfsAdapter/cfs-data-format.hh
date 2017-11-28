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

// Icarous Command types
typedef enum {
    _LOOKUP_,
    _COMMAND_,
    _LOOKUP_RETURN_,
    _COMMAND_RETURN_,
} messageType_t;

// Icarous return types
typedef enum{
    _REAL_,
    _INTEGER_,
    _BOOLEAN_,
    _REAL_ARRAY_,
    _INTEGER_ARRAY_,
    _BOOLEAN_ARRAY_
}returnType_t;

typedef struct{
    messageType_t mType;
    returnType_t rType;
    int id;
    char name[50];
    double argsD[4];
    bool argsB[4];
    int argsI[4];
    char string[50];
}PlexilCommandMsg;

#ifdef __cplusplus
}
#endif


#endif //CFS_DATA_FORMAT_H
