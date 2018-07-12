/**
 * @file tracking_msg.h
 * @brief message definitions for tracking application
 */

#ifndef ICAROUS_CFS_TRACKING_MSG_H
#define ICAROUS_CFS_TRACKING_MSG_H

#include "cfe.h"

/**
 * @defgroup TRACKING_MESSAGES
 * @brief Tracking message definitions
 * @ingroup TRACKING
 * @ingroup MESSAGES
 * @{
 */

typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
    int32_t trackingObjectID[10];            /**< IDs of objects for which resolutions are computed. */
    double trackingVelocity[10][3];          /**< tracking resolutions: Track (deg), ground speed and vertical speed in m/s */
    double trackingHeading[10];              /**< tracking heading in degrees */
}trackingResolution_t;


#endif //ICAROUS_CFS_TRACKING_MSG_H
