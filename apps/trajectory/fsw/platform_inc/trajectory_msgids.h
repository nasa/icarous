/**
 * @file trajectory_msgids.h
 * @brief definition of message ids relevant to the trajectory applicaiton
 */
#ifndef ICAROUS_CFS_TRAJECTORY_MSGIDS_H
#define ICAROUS_CFS_TRAJECTORY_MSGIDS_H

#define ICAROUS_TRAJECTORY_MID         0x0462   ///< Encodes the result of trajectory computation. message type: flightplan_t
#define EUTL1_TRAJECTORY_MID           0x0463    ///< A trajectory encoded as an E-UTIL string
#define EUTL2_TRAJECTORY_MID           0x0464    ///< A trajectory encoded as an E-UTIL string
#define ICAROUS_TRAJECTORY_REQUEST_MID 0x0465   ///< Request to compute a new trajectory: message type: trajectory_request_t
#define FLIGHTPLAN_MONITOR_MID         0x0466   ///< Flight plan monitoring information. message type: flightplan_monitor_t
#define TRAJECTORY_PARAMETERS_MID      0x0904   ///< Trajectory parameters

/**@}*/
#endif //ICAROUS_CFS_TRAJECTORY_MSGIDS_H
