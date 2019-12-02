/**
 * @file safe2ditch_msg.h
 * @brief safe2ditch app message definition
 */

#ifndef ICAROUS_CFS_SAFE2DITCH_MSG_H
#define ICAROUS_CFS_SAFE2DITCH_MSG_H


#include "cfe.h"
#include "stdbool.h"

/**
 * @defgroup SAFE2DITCH_MESSAGES
 * @brief safe2ditch message definitions
 * @ingroup SAFE2DITCH
 * @ingroup MESSAGES
 * @{
 */

/**
 * @struct safe2ditchStatus_t
 * @brief message contaning status information about safe2ditch
 *
 * Published under the SAFE2DITCH_STATUS_MID topic
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
    double ditchsite[3];                     /**< Ditch site selected by safe2ditch */
    bool resetDitch;                         /**< Reset ditching */
    bool endDitch;                           /**< End ditching */
    bool ditchGuidanceRequired;              /**< Ditch guidance required */
    bool ditchRequested;                     /**< Ditching requested */
}safe2ditchStatus_t;




#endif //ICAROUS_CFS_SAFE2DITCH_MSG_H
