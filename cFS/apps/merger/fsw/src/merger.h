/**
 * @file merger.h
 * @brief Definition of defines, structs and functions for merger app
 *
 * @author Swee Balachandran
 *
 */
#ifndef ICAROUS_CFS_MERGER_H
#define ICAROUS_CFS_MERGER_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include <string.h>
#include <stdbool.h>

#include "msgids/ardupilot_msgids.h"
#include "msgdef/ardupilot_msg.h"
#include "msgdef/merger_msg.h"
#include "merger_table.h"

#define MERGER_STARTUP_INF_EID 0
#define MERGER_COMMAND_ERR_EID 1
#define MERGER_RECEIVED_FENCE_EID 3

#define MERGER_PIPE_DEPTH 100
#define MERGER_PIPE_NAME "MERGER_PIPE"
#define MERGER_MAJOR_VERSION 1
#define MERGER_MINOR_VERSION 0


/**
 * @defgroup MERGER 
 * @brief Provides merging and spacing capability for intersections
 * @ingroup APPLICATIONS
 *
 *
 * @see MERGER_MESSAGES, MERGER_MESSAGE_TOPICS, MERGER_TABLES
 */


typedef enum{
    _MERGER_NOOP_,
    _MERGER_TAKECONTROL_,
    _MERGER_SETNEWPOS_,
    _MERGER_TRANSITION_,
    _MERGER_GIVECONTROL_
}localMergerNavStates_t;

/**
 * @struct mergerAppData_t
 * @brief  Struct to hold merger app data
 */
typedef struct{
    CFE_SB_PipeId_t    Merger_Pipe;       ///< Pipe variable
    CFE_SB_MsgPtr_t    Merger_MsgPtr;     ///< Pointer to SB message
    CFE_TBL_Handle_t   Merger_tblHandle;  ///< Table handle
    double position[3];                   ///< current position
    double velocity[3];                   ///< current velocity
    double currentSpeed;                  ///< current speed
    double entryFlightPlan[10][3];        ///< flight plan used to enter intersection.
    bool entryFlightPlanUpdated;          ///< entry flight plan updated.
    bool defaultEntryPlan;                ///< default entry plan
    double map[5][3];                     ///< Intersection information
    arrivalData_t arrivalData;            ///< arrival data
    bool arrivalDataUpdated;              ///< true if new arrival data exists
    MergerTable_t mergerTable;            ///< merger table data
    int nextIntersectionId;               ///< Next intersection id
    double entryPoint[3];                 ///< Entry point
    double dist2ZoneEntry;                ///< Distance to entry point
    uint32_t time2ZoneEntry;              ///< Time to entry
    arrivalData_t intersectionEntryData[INTERSECTION_MAX];   ///< Intersection entry data
    int numNodesInt;
    int aircraftIdInt[10];
    double newWaypoint[3];                ///< New detour waypoint
    double mergingSpeed;
    double detourDist;
    bool entryPointComputed;
    bool executePath;
    bool inComputeZone;
    uint8_t waypointIndex;
    double nextNavWaypoint[3];
    double dist2NextWaypoint;
    double speed2NextWaypoint;
    double maneuveringSpeed;
    localMergerNavStates_t navState;

}mergerAppData_t;

/**
 * App's main entry point
 */
void MERGER_AppMain(void);

/**
 * App initialization
 */
void MERGER_AppInit(void);

/**
 * App clean up
 */
void MERGER_AppCleanUp(void);

/**
 * Function to process received packets
 */
void MERGER_ProcessPacket(void);


/**
 * Function to validate table parameters
 * @param TblPtr pointer to table
 * @return 0 if success
 */
int32_t MergerTableValidationFunc(void *TblPtr);

#endif //ICAROUS_CFS_MERGER_H
