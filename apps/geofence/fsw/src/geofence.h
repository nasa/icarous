/**
 * @file geofence.h
 * @brief Definition of defines, structs and functions for geofence monitoring app
 *
 * @author Swee Balachandran
 *
 */


#ifndef ICAROUS_CFS_GEOFENCE_H
#define ICAROUS_CFS_GEOFENCE_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"
#include <string.h>


#include "GeofenceMonitor.h"
#include "Icarous_msgids.h"
#include "Icarous_msg.h"
#include "geofence_tbl.c"
#include "geofence_msg.h"
#include "geofence_msgids.h"
#include "trajectory_msgids.h"
#include "sch_msgids.h"

#define GEOFENCE_STARTUP_INF_EID 0
#define GEOFENCE_COMMAND_ERR_EID 1
#define GEOFENCE_RECEIVED_FENCE_EID 3

#define GEOFENCE_PIPE_DEPTH 100
#define GEOFENCE_PIPE_NAME "GEOFENCE_PIPE"
#define GEOFENCE_MAJOR_VERSION 1
#define GEOFENCE_MINOR_VERSION 0

/**
 * @defgroup GEOFENCE_MONITOR_cFS_APP
 * @ingroup CFSAPPS
 *
 * @details This application provides geofence monitoring servies. Both keep-in and keep-out constraint violations are monitored.
 * This application listens to the ICAROUS_GEOFENCE_MID topic for geofence_t messages encoding geofence information.
 * It publishes output on the ICAROUS_GEOFENCE_MONITOR_MID.  Additionally, it also checks if each mission waypoint
 * satisfies all geofence constraints. It also checks if the direct path from a given position to the next waypoint is
 * free from geofence conflicts.
 *
 */

/**
 * @struct geofenceAppData_t
 * @brief  Struct to hold geofence app data
 */
typedef struct{
    CFE_SB_PipeId_t    Geofence_Pipe;       ///< Pipe variable
    CFE_SB_MsgPtr_t    Geofence_MsgPtr;     ///< Pointer to SB message
    CFE_TBL_Handle_t   Geofence_tblHandle;  ///< Table handle
    void* gfMonitor;                        ///< Opaque pointer to GeofenceMonitor
    double position[3];                     ///< Current lat[deg], lon[deg], alt[m] position
    double velocity[3];                     ///< Current velocity: trk (deg), ground speed (m/s) and vertical speed (m/s)
    geofenceConflict_t gfConflictData;      ///< Geofence conflict
    flightplan_t flightplan1;               ///< Primary Flight plan to monitor
    flightplan_t flightplan2;               ///< Secondary Flight plan to monitor
    uint16_t numFences;                     ///< Number of fnces
    bool receivedFP1;                       ///< Monitor flag for flight plan 1
    bool receivedFP2;                       ///< Monitor flag for flight plan 2
    bool receivedFence;                     ///< Flag indicating fence received
    bool waypointConflict1[50];             ///< Geofence conflict for each waypoint in mission flight plan
    bool directPathToWP1[50];               ///< Flag indicating if direct path to waypoint exists. Array index identifies the waypoint index.
    bool waypointConflict2[50];             ///< Geofence conflict for each waypoint in mission flight plan
    bool directPathToWP2[50];               ///< Flag indicating if direct path to waypoint exists. Array index identifies the waypoint index.
}geofenceAppData_t;

/**
 * App's main entry point
 */
void GEOFENCE_AppMain(void);

/**
 * App initialization
 */
void GEOFENCE_AppInit(void);

/**
 * App clean up
 */
void GEOFENCE_AppCleanUp(void);

/**
 * Function to process received packets
 */
void GEOFENCE_ProcessPacket(void);

/**
 * @var geofenceAppData
 * @brief global variable to hold app data
 */
geofenceAppData_t geofenceAppData;

/**
 * Function to validate table parameters
 * @param TblPtr pointer to table
 * @return 0 if success
 */
int32_t GeofenceTableValidationFunc(void *TblPtr);

#endif //ICAROUS_CFS_GEOFENCE_H
