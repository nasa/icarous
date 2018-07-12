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


#define INTERFACE_GS_WAKEUP_MID 0x0F01    ///< wake up autopilot communication thread.
#define INTERFACE_AP_WAKEUP_MID 0x0F02    ///< wake up ground staion communication thread.
#define GEOFENCE_WAKEUP_MID 0x0F03        ///< wake up geofence monitor.
#define TRAFFIC_WAKEUP_MID 0x0F04         ///< wake up traffic monitoring functionality
#define TRAJECTORY_WAKEUP_MID 0x0F05      ///< wake up trajectory monitoring funcitonalities
#define PLEXIL_WAKEUP_MID 0x0F06          ///< wake up plexil
#define TRACKING_WAKEUP_MID 0x0F07        ///< wake up tracking

/**@}*/
#endif //ICAROUS_CFS_SCHEDULER_MSGIDS_H
