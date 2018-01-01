//
// Created by Swee Balachandran on 12/22/17.
//

#ifndef ICAROUS_CFS_TRAFFIC_H
#define ICAROUS_CFS_TRAFFIC_H

/// Required header files.
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include <string.h>

#include "TrafficMonitor_proxy.h"
#include "msgids/msgids.h"
#include "Plexil_msg.h"

#define TRAFFIC_STARTUP_INF_EID 0
#define TRAFFIC_COMMAND_ERR_EID 1
#define TRAFFIC_PIPE_DEPTH 25
#define TRAFFIC_PIPE_NAME "TRAFFIC_PIPE"
#define TRAFFIC_MAJOR_VERSION 1.0
#define TRAFFIC_MINOR_VERSION 0.0

/**
 * \struct TrajectoryAppData
 * Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    Traffic_Pipe;       ///< pipe variable
    CFE_SB_MsgPtr_t    Traffic_MsgPtr;     ///< msg pointer to SB message
    FlightData* fdata;                      ///< opaque pointer to store FlightDataManager
    TrafficMonitor* pplanner;             ///< opaque pointer to TrafficMonitor
}TrafficAppData_t;


void TRAFFIC_AppMain(void);

void TRAFFIC_AppInit(void);

void TRAFFIC_AppCleanUp();

void TRAFFIC_ProcessPacket();

void TrafficPlxMsgHandler(plexil_interface_t* msg);

TrafficAppData_t TrafficAppData;
plexil_interface_t trafficPlexilMsg;


#endif //ICAROUS_CFS_TRAFFIC_H