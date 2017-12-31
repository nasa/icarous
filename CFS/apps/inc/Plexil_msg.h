//
// Created by research133 on 11/27/17.
//

#ifndef PLEXIL_MSG_H_H
#define PLEXIL_MSG_H_H

#include <stdint.h>
#include "cfe.h"
#include "cfs-data-format.hh"
#include "PlexilWrapper.h"

typedef struct{
    uint8_t TlmHeader[CFE_SB_CMD_HDR_SIZE];
    PlexilMsg plxData;
}plexil_interface_t;


#endif //CFETOP_PLEXIL_MSG_H_H
