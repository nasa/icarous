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

/**
 * @struct trackingResolution_t
 * @brief tracking commands
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
    int32_t trackingObjectID[10];            /**< IDs of objects for which resolutions are computed. */
    double trackingVelocity[10][3];          /**< tracking resolutions: Track (deg), ground speed and vertical speed in m/s */
    double trackingHeading[10];              /**< tracking heading in degrees */
}trackingResolution_t;

/**
 * @struct tracking_parameters_t
 * @brief tracking parameter definition
 */
typedef struct{
   uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];
   bool command;        ///< command mode (true/false). Send commands to ardupilot directly.
   int trackingObjId;   ///< Id of object to track;
   double pGainX;       ///< proportional gain x;
   double pGainY;       ///< proportional gain y;
   double pGainZ;       ///< proportional gain z;
   double heading;      ///< heading behind the target from which to track (degree).
   double distH;        ///< horizontal distance to maintain from the target (m).
   double distV;        ///< vertical distance to maintain from the target (m).
}tracking_parameters_t;

#endif //ICAROUS_CFS_TRACKING_MSG_H
