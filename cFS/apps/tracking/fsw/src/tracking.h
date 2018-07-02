/**
 * @file tracking.h
 * @brief Function declaration, MACRO and data structure definitions for the tracking application
 */

#ifndef ICAROUS_CFS_TRACKING_H
#define ICAROUS_CFS_TRACKING_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include <string.h>

#include "msgids/ardupilot_msgids.h"
#include "msgids/scheduler_msgids.h"
#include "msgdef/ardupilot_msg.h"
#include "tracking_tbl.h"

#define TRACKING_STARTUP_INF_EID 0
#define TRACKING_COMMAND_ERR_EID 1
#define TRACKING_RECEIVED_OBJECT_EID 2

#define TRACKING_PIPE_DEPTH 100
#define TRACKING_PIPE_NAME "TRACKING_PIPE"
#define TRACKING_MAJOR_VERSION 1
#define TRACKING_MINOR_VERSION 0

/**
 * @defgroup TRAFFIC
 * @brief Application to track object
 * @ingroup APPLICATIONS
 *
 * @details
 *
 * @see TRACKING_MESSAGES, TRACKING_MESSAGE_TOPICS, TRACKING_TABLES
 */


/**
 * @struct TrackingAppData_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    Tracking_Pipe;       ///< Pipe variable
    CFE_SB_MsgPtr_t    Tracking_MsgPtr;     ///< Msg pointer to SB message
    CFE_TBL_Handle_t   Tracking_tblHandle;  ///< Traffic table handle
    double position[3];                     ///< Current position as lat (deg), lon (deg) and alt (m)
    double velocity[3];                     ///< Current velocity as track (deg), ground speed (m/s) and vertical speed (m/s)
    double objectsPos[10][3];               ///< Positions of objects to track:  lat (deg), lon (deg) and alt (m)
    double objectsVel[10][3];               ///< Velocity of objects to track. Vn, Ve, Vd (m/s)
    int objectID[10];                       ///< Object id
    int numObjects;                         ///< total number of objects added.
    double trackingVel[10][3];              ///< tracking velocity: Track (deg), ground speed (m/s) and vertical speed (m/s)
    trackingResolution_t resolution;        ///< computed tracking resolution.
    double propGain[3];                     ///< propotional gain applied to (Vn,Ve and Vd)
    int trackingID;                         ///< id of object being tracked.
    double heading;                         ///< heading with respect to target from which tracking is done.
    double distH;                           ///< horizontal separation to maintain from target.
    double distV;                           ///< vertical separation to maintain from target.
    bool command;                           ///< command mode. (true/false) send commands to ardupilot if true.
    bool track;                             ///< track (true/false): enable tracking.
}TrackingAppData_t;

/**
 * App entry point
 */
void TRACKING_AppMain(void);

/**
 * Initializes data structures and cfs related entities.
 */
void TRACKING_AppInit(void);

/**
 * Deallocates memory
 */
void TRACKING_AppCleanUp();

/**
 * Processes incoming software bus messages
 */
void TRACKING_ProcessPacket();

/**
 * Validates table parameters
 * @param TblPtr Pointer to table
 * @return  0 if success
 */
int32_t TrackingTableValidationFunc(void *TblPtr);

/**
 * @var Global variable to store all app related data
 */
TrackingAppData_t trackingAppData;

#endif //ICAROUS_CFS_TRACKING_H