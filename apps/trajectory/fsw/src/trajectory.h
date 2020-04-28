/**
 * @file trajectory.h
 * @brief Function declarations, data structure and macro declarations for the trajectory management application
 */
#ifndef ICAROUS_CFS_TRAJECTORY_H
#define ICAROUS_CFS_TRAJECTORY_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"

#include <string.h>

#include "Icarous.h"
#include "TrajectoryPlanner_proxy.h"
#include "sch_msgids.h"
#include "trajectory_msgids.h"
#include "traffic_msg.h"
#include "traffic_msgids.h"
#include "Icarous_msg.h"
#include "trajectory_msg.h"
#include "trajectory_tbl.h"

#define TRAJECTORY_STARTUP_INF_EID 0
#define TRAJECTORY_COMMAND_ERR_EID 1
#define TRAJECTORY_RECEIVED_INTRUDER_EID 2
#define TRAJECTORY_RECEIVED_FLIGHTPLAN_EID 3
#define TRAJECTORY_RECEIVED_GEOFENCE_EID 4

#define TRAJECTORY_PIPE_DEPTH 100
#define TRAJECTORY_PIPE_NAME "TRAJECTORY_PIPE"
#define TRAJECTORY_SCH_PIPE_NAME "TRAJ_SCH_PIPE"
#define TRAJECTORY_SCH_PIPE_DEPTH 100
#define TRAJECTORY_MAJOR_VERSION 1
#define TRAJECTORY_MINOR_VERSION 0

#define TRAJ_MONITOR_TASK_ID 23
#define TRAJ_MONITOR_TASK_STACK_SIZE 1024
#define TRAJ_MONITOR_PRIORITY 65


uint32 traj_monitor_stack[TRAJ_MONITOR_TASK_STACK_SIZE];
uint32 traj_monitor_task_id;

/**
 * @defgroup TRAJECTORY_MONITOR
 * @brief Trajectory monitoring application
 * @ingroup APPLICATIONS
 *
 * @details This application provides trajectory monitoring and management services. Trajectory monitoring is done on the
 * uploaded mission flight plan from a ground control station. The misison flight plan is primarily monitored for
 * cross-track deviaiton. Other metrics such as distance to the next waypoint waypoint, heading to intercept flight plan
 * enroute to next waypoint, resolution maneuver to intercept flight plan are also computed. This information is
 * published at a rate controlled by the cFS scheduler.
 *
 * This application also provides path computation services. It subscribes to a trajectory request message that encodes
 * the initial and final conditions of the path. A path is computed using the requested algorithm. After a path is
 * computed the resulting trajectory is published on the cFS software bus. Properties of the search algorithms used are
 * specified in the input tables.
 *
 * @see TRAJECTORY_MESSAGES, TRAJECTORY_MESSAGE_TOPICS, TRAJECTORY_TABLES
 */


/**
 * @struct TrajectoryAppData_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    TrajRequest_Pipe;       ///< Pipe variable
    CFE_SB_PipeId_t    TrajData_Pipe;          ///< Pipe variable
    CFE_SB_MsgPtr_t    TrajRequest_MsgPtr;     ///< Msg pointer to SB message
    CFE_SB_MsgPtr_t    Traj_MsgPtr;            ///< Scheduler message
    CFE_TBL_Handle_t   Trajectory_TblHandle;   ///< Table pointer
    PathPlanner* pplanner;                     ///< Opaque pointer to TrajectoryMonitor
    int numPlansComputed;                      ///< Total number of plans computed
    int runThreads;                            ///< Flag to signals to run/stop
    double position[3];                        ///< Position as lat (deg), lon (deg) and alt (m)
    double velocity[3];                        ///< Velocity ad trk (deg), ground speed (m/s) and vertical speed (m/s)
    uint32_t mutexAcState;                     ///< Mutex to enforce synchronization on shared variables
    int nextWP1;                               ///< Next waypoint index in flightplan 1
    int nextWP2;                               ///< Next waypoint index in flightplan 2
    char planID[10];                           ///< Current plan ID
    bool monitor;                              ///< Flag indicating when monitoring should begin.
    bool updateDAAParams;                      ///< Update DAA params via SB messages
    flightplan_t flightplan1;                  ///< primary flightplan to monitor
    flightplan_t flightplan2;                  ///< secondary flightplan to monitor
    double xtrkDev;                            ///< xtrk deviation allowed
    double xtrkGain;                           ///< xtrk gain
    double resSpeed;                           ///< resolution speed
    algorithm_e searchType;                    ///< preferred search algorithm
    bool eutlReceived;                         ///< eutl plan received check
    flightplan_monitor_t fpMonitor;            ///< flight plan monitor
    trajectory_parameters_t trajParams;
}TrajectoryAppData_t;

/**
 * Trajectory app entry point
 */
void TRAJECTORY_AppMain(void);

/**
 * Trajectory app initialization
 */
void TRAJECTORY_AppInit(void);

/**
 * Trajectory app data initialization
 */
void TRAJECTORY_AppInitData(TrajectoryTable_t* TblPtr);

/**
 * Clean up any memory allocated.
 */
void TRAJECTORY_AppCleanUp();

/**
 * Process software bus messages
 */
void TRAJECTORY_ProcessPacket();

/**
 * Peform the trajectory monitoring functions
 */
void TRAJECTORY_Monitor(void);

/**
 * Validation app table parameters
 * @param TblPtr point to app table
 * @return  0 if success
 */
int32_t TrajectoryTableValidationFunc(void *TblPtr);

/**
 * @var Global variable holding all app data
 */
TrajectoryAppData_t TrajectoryAppData;


#endif //ICAROUS_CFS_TRAJECTORY_H
