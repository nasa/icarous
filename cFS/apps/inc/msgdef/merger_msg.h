/**
 * @file merger_msg.h
 * @brief Defintion of messages used by merger_msg.h
 */

#ifndef ICAROUS_CFS_MERGER_MSG_H
#define ICAROUS_CFS_MERGER_MSG_H

#include <cfe.h>
#include <stdint.h>

/**
 * @defgroup MERGER_MESSAGES
 * @brief Merger message definitions
 * @ingroup MERGER
 * @ingroup MESSAGES
 * @{
 */

#define INTERSECTION_MAX 5

typedef struct arrivalData{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];
    int aircraftID;
    int intersectionID;
    uint32_t earlyArrivalTime;
    uint32_t currentArrivalTime;
    uint32_t lateArrivalTime;
}arrivalData_t;


typedef struct Raft_LogAcc {
    CFE_SB_TlmHdr_t hdr;
    int32 term;
    int32 node_id;
    int32 index;
    int32 entries;
    int32 numNodes;
    arrivalData_t log_ud_acc[INTERSECTION_MAX];            //ICAROUS
}log_ud_acc_t;


#endif //ICAROUS_CFS_MERGER_MSG_H
