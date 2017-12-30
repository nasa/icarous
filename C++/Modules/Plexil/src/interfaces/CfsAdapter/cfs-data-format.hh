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
#include <endian.h>

#if __BYTE_ORDER == __BIG_ENDIAN

// No translation needed for big endian system

#define Swap2Bytes(val) val
#define Swap4Bytes(val) val
#define Swap8Bytes(val) val
#else

// Swap 2 byte, 16 bit values:
#define Swap2Bytes(val) \
( (((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00) )

// Swap 4 byte, 32 bit values:
#define Swap4Bytes(val) \
( (((val) >> 24) & 0x000000FF) | (((val) >>  8) & 0x0000FF00) | \
(((val) <<  8) & 0x00FF0000) | (((val) << 24) & 0xFF000000) )

// Swap 8 byte, 64 bit values:
#define Swap8Bytes(val) \
( (((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | \
(((val) >> 24) & 0x0000000000FF0000) | (((val) >>  8) & 0x00000000FF000000) | \
(((val) <<  8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | \
(((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000) )
#endif

#define PLEXIL_MSG_BUFFER 250

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
    _BOOLEAN_ARRAY_,
    _STRING_
}dataType_t;

typedef struct{
    messageType_t mType;
    dataType_t rType;
    int id;
    char name[50];
    char buffer[PLEXIL_MSG_BUFFER];
}PlexilMsg;

#ifdef __cplusplus
}
#endif


#endif //CFS_DATA_FORMAT_H
