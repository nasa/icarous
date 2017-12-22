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

#include "FlightDataManager.h"

#define GEOFENCE_STARTUP_INF_EID 0
#define GEOFENCE_COMMAND_ERR_EID 1

/**
 * \struct geofenceAppData
 * Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    Geofence_Pipe;       ///< pipe variable
    CFE_SB_MsgPtr_t    Geofence_MsgPtr;     ///< msg pointer to SB message

    struct plexilExec* exec;
    struct plexilInterfaceAdapter* adap;
}geofenceAppData_t;



void Geofence_AppMain(void);

void Geofence_AppInit(void);

void Geofence_AppCleanUp();

void Geofence_ProcessPacket();

void Geofence_Run();


#endif //ICAROUS_CFS_GEOFENCE_H
