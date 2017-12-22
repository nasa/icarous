//
// Created by Swee Balachandran on 12/22/17.
//

#ifndef ICAROUS_CFS_GEOFENCE_H
#define ICAROUS_CFS_GEOFENCE_H

/// Required header files.
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include <string.h>

#include "GeofenceMonitor_proxy.h"
#include "msgids/msgids.h"
#include "Plexil_msg.h"

#define GEOFENCE_STARTUP_INF_EID 0
#define GEOFENCE_COMMAND_ERR_EID 1
#define GEOFENCE_PIPE_DEPTH 25
#define GEOFENCE_PIPE_NAME "GEOFENCE_PIPE"
#define GEOFENCE_MAJOR_VERSION 1.0
#define GEOFENCE_MINOR_VERSION 0.0

/**
 * \struct geofenceAppData
 * Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    Geofence_Pipe;       ///< pipe variable
    CFE_SB_MsgPtr_t    Geofence_MsgPtr;     ///< msg pointer to SB message
    FlightData* fdata;                      ///< opaque pointer to store FlightDataManager
    GeofenceMonitor* gfMonitor;             ///< opaque pointer to GeofenceMonitor
}geofenceAppData_t;


void Geofence_AppMain(void);

void Geofence_AppInit(void);

void Geofence_AppCleanUp();

void Geofence_ProcessPacket();

void Geofence_Run();

geofenceAppData_t geofenceAppData;
plexil_interface_t gfPlexilMsg;


#endif //ICAROUS_CFS_GEOFENCE_H