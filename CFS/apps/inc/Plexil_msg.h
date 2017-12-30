//
// Created by research133 on 11/27/17.
//

#ifndef PLEXIL_MSG_H_H
#define PLEXIL_MSG_H_H

#include <stdint.h>
#include "cfe.h"
#include "cfs-data-format.hh"

typedef struct{
    uint8_t TlmHeader[CFE_SB_CMD_HDR_SIZE];
    PlexilCommandMsg plxMsg;
}plexil_interface_t;

#define CHECK_MSG(msg,string) !strcmp(msg->plxMsg.name,string)

#endif //CFETOP_PLEXIL_MSG_H_H
