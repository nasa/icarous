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


#define FREQ_50_WAKEUP_MID 0x0F01    ///< wake up scheduler message generated at  50 Hz
#define FREQ_30_WAKEUP_MID 0x0F02    ///< wake up scheduler message generated at  30 Hz
#define FREQ_10_WAKEUP_MID 0x0F03    ///< wake up scheduler message generated at  10 Hz
#define FREQ_01_WAKEUP_MID 0x0F04    ///< wake up scheduler message generated at  01 Hz

/**@}*/
#endif //ICAROUS_CFS_SCHEDULER_MSGIDS_H
