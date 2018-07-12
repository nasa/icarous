/**
 * @file geofence_msgids.h
 * @brief Defines message topics published by the geofence application.
 */

#ifndef ICAROUS_CFS_GEOFENCE_MSGIDS_H
#define ICAROUS_CFS_GEOFENCE_MSGIDS_H


/**
 * @defgroup GEOFENCE_MESSAGE_TOPICS
 * @brief Geofence message topics
 * @ingroup GEOFENCE_MONITOR
 * @ingroup TOPICS
 * @{
 */

#define ICAROUS_GEOFENCE_MONITOR_MID 0x0240    ///< Geofence monitor topic. message type: geofenceConflict_t
#define GEOFENCE_PATH_CHECK_MID 0x0235         ///< Geofence path checking request. message type: pathFeasibilityCheck_t
#define GEOFENCE_PATH_CHECK_RESULT_MID 0x0236  ///< Geofence path checking result. pathFeasibilityCheck_t

/**@}*/
#endif //ICAROUS_CFS_GEOFENCE_MSGIDS_H
