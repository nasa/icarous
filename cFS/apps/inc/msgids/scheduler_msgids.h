/**
 * @file scheduler_msgids.h
 * @brief message topics published by the scheduler
 */
#ifndef ICAROUS_CFS_SCHEDULER_MSGIDS_H
#define ICAROUS_CFS_SCHEDULER_MSGIDS_H

/**
 * @defgroup SCHEDULER
 * @brief Generated cFS messages at prescribed intervals
 * @ingroup APPLICATIONS
 */

/**
 * @defgroup SCHEDULER_TOPICS
 * @brief description of topics published by scheduler
 * @ingroup SCHEDULER
 * @ingroup TOPICS
 * @{
 */


#define INTERFACE_GS_WAKEUP_MID 0x001F    ///< wake up autopilot communication thread.
#define INTERFACE_AP_WAKEUP_MID 0x0020    ///< wake up ground staion communication thread.
#define GEOFENCE_WAKEUP_MID 0x021E        ///< wake up geofence monitor.
#define TRAFFIC_WAKEUP_MID 0x0335         ///< wake up traffic monitoring functionality
#define TRAJECTORY_WAKEUP_MID 0x0464      ///< wake up trajectory monitoring funcitonalities
#define PLEXIL_WAKEUP_MID 0x0100          ///< wake up plexil

/**@}*/
#endif //ICAROUS_CFS_SCHEDULER_MSGIDS_H
