/**
 * @file geofence_msg.h
 * @brief Defintion of messages used by geofence_msg.h
 */

#ifndef ICAROUS_CFS_GEOFENCE_MSG_H
#define ICAROUS_CFS_GEOFENCE_MSG_H

#include "cfe.h"

/**
 * @defgroup GEOFENCE_MESSAGES
 * @brief Geofence message definitions
 * @ingroup GEOFENCE_MONITOR
 * @ingroup MESSAGES
 * @{
 */

/**
 * @struct geofenceConflict_t
 * @brief message contaning information about goefence conflicts
 *
 * Published under the ICAROUS_GEOFENCE_MONITOR_MID topic
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
    uint8_t numConflicts;                    /**< number of conflicts */
    uint16_t numFences;                      /**< total fences */
    uint8_t conflictFenceIDs[5];             /**< conflicting fence ids */
    uint8_t conflictTypes[5];                /**< type of conflict (keep in: 0/ keep out:1) */
    double timeToViolation[5];               /**< time to violating constraint at current speed */
    double recoveryPosition[3];              /**< recovery position */
    bool waypointConflict[50];               /**< 0 if waypoint is conflict free. 1 if waypoint conflict due to geofence */
    bool directPathToWaypoint[50];           /**< 1 if direct path exists, 0 if no direct path exist */
}geofenceConflict_t;

/**
 * @struct pathFeasibilityCheck_t
 * @brief request to check if a path is feasible between two given points. Also used as a response.
 *
 * Requests are published under the GEOFENCE_PATH_CHECK_MID topic.
 * Responses are published under the GEOFENCE_PATH_CHECK_RESULT_MID topic.
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
    double fromPosition[3];                  /**< Initial position in lat (deg), lon (deg) and alt (m) */
    double toPosition[3];                    /**< To position in lat (deg), lon (deg) and alt (m) */
    bool feasibleA;                          /**< true if from position is free from conflict */
    bool feasibleB;                          /**< true if to position is free from conflict */
    bool feasibleAB;                         /**< true if path is feasible (free from geofence conflicts) */
}pathFeasibilityCheck_t;

/**
 * @struct geofence_parameters_t
 * @brief data structure containing information about the parameters used by the geofence app
 */
typedef struct{
   uint8_t  TlmHeader[CFE_SB_TLM_HDR_SIZE];
   double lookahead;
   double hthreshold;
   double vthreshold;
   double hstepback;
   double vstepback;
}geofence_parameters_t;

/**@}*/
#endif //ICAROUS_CFS_GEOFENCE_MSG_H
