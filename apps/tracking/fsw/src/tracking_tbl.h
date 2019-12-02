/**
 * @file tracking_tbl.h
 * @brief tracking application table definition
 */

#ifndef ICAROUS_CFS_TRACKING_TBL_H
#define ICAROUS_CFS_TRACKING_TBL_H

#include <stdbool.h>

/**
 * @defgroup TRACKING_TABLES
 * @brief tracking tables
 * @ingroup TABLES
 */

/**
 * @struct TrackingTable_t
 * @brief input table parameters for tracking application
 * @ingroup TRACKING_TABLES
 */
typedef struct
{
    bool command;        ///< command mode (true/false). Send commands to ardupilot directly.
    int trackingObjId;   ///< Id of object to track;
    double pGainX;       ///< proportional gain x;
    double pGainY;       ///< proportional gain y;
    double pGainZ;       ///< proportional gain z;
    double heading;      ///< heading behind the target from which to track (degree).
    double distH;        ///< horizontal distance to maintain from the target (m).
    double distV;        ///< vertical distance to maintain from the target (m).
}TrackingTable_t;


#endif //ICAROUS_CFS_TRACKING_TBL_H
