//
// Created by Swee Balachandran on 12/22/17.
//

#ifndef ICAROUS_CFS_Trajectory_H
#define ICAROUS_CFS_Trajectory_H

/// Required header files.
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include <string.h>

#include "TrajectoryMonitor_proxy.h"
#include "msgids/msgids.h"
#include "Plexil_msg.h"

#define Trajectory_STARTUP_INF_EID 0
#define Trajectory_COMMAND_ERR_EID 1
#define Trajectory_PIPE_DEPTH 25
#define Trajectory_PIPE_NAME "Trajectory_PIPE"
#define Trajectory_MAJOR_VERSION 1.0
#define Trajectory_MINOR_VERSION 0.0

/**
 * \struct TrajectoryAppData
 * Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    Trajectory_Pipe;       ///< pipe variable
    CFE_SB_MsgPtr_t    Trajectory_MsgPtr;     ///< msg pointer to SB message
    FlightData* fdata;                      ///< opaque pointer to store FlightDataManager
    TrajectoryMonitor* gfMonitor;             ///< opaque pointer to TrajectoryMonitor
}TrajectoryAppData_t;


void Trajectory_AppMain(void);

void Trajectory_AppInit(void);

void Trajectory_AppCleanUp();

void Trajectory_ProcessPacket();

void Trajectory_Run();

TrajectoryAppData_t TrajectoryAppData;
plexil_interface_t gfPlexilMsg;


#endif //ICAROUS_CFS_Trajectory_H