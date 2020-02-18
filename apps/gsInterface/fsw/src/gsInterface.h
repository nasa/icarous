/**
 * @file gsInterface.h
 * @brief function declarations, definitions of macros, datastructures and global variables for the ground station interface application
 */


#ifndef _gsInterface_h_
#define _gsInterface_h_

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <network_includes.h>
#include "Icarous.h"

#include "sch_msgids.h"
#include "gsInterface_table.h"

#ifdef APPDEF_TRAFFIC
#include "traffic_msg.h"
#include "traffic_msgids.h"
#endif

#ifdef APPDEF_TRACKING
#include "tracking_msg.h"
#include "tracking_msgids.h"
#endif 

#ifdef APPDEF_TRAJECTORY
#include "trajectory_msg.h"
#include "trajectory_msgids.h"
#endif 

#ifdef APPDEF_GEOFENCE
#include "geofence_msg.h"
#include "geofence_msgids.h"
#endif

#ifdef APPDEF_ROTORSIM
#include "rotorsim_msg.h"
#include "rotorsim_msgids.h"
#endif

#ifdef APPDEF_MERGER
#include "merger_msg.h"
#include "merger_msgids.h"
#endif

#include "mavlink/ardupilotmega/mavlink.h"
#include "port_lib.h"


#define BUFFER_LENGTH 1000  ///< Mavlink message receive buffer size

#define GSINTERFACE_PIPE_NAME "GSINTF_PIPE"
#define GSINTERFACE_PIPE_DEPTH 100

#define SCH_GSINTERFACE_PIPE1_NAME "SCH_GSINTERFACE"


/**
 * @defgroup GSINTERFACE
 * @brief An application to interface cFS with an ground station system
 * @ingroup APPLICATIONS
 *
 * @details This application opens two ports, one port connects to the autopilot and the other connects to the ground station.
 * The properties of these ports are defined in the input table. Mavlink packets received from the autopilot are
 * redirected to the ground station and vice versa. Additionally, required information from incoming mavlink messages
 * are extracted and published on to the cFS software bus so that other cFS applications can make use of them.
 * Furthermore, this application also subscribes for commands to the autopilot issued by other cFS applications and
 * converts them to appropriate mavlink messages and sends them to the autopilot. In addition to the autopilot's
 * telemetry that is being piped to the ground station, some Icarous related data (encoded as mavlink messages) is also
 * sent to the ground station. Reading data from the autopilot and groundstation are each on separate threads. The rate
 * at which each thread executes is controlled by the cFS scheduler.
 *
 * @see ARDUPILOT_MESSAGES, ARDUPILOT_MESSAGE_TOPICS, ARDUPILOT_TABLES
 */


/**
 * @struct appdataIntGS_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    INTERFACE_Pipe;      ///< pipe variable
    CFE_SB_PipeId_t    SchInterface_Pipe;  ///< pipe variable
    CFE_SB_MsgPtr_t    INTERFACEMsgPtr;     ///< msg pointer to SB message
    CFE_SB_MsgPtr_t    Sch_MsgPtr;         ///< msg pointer to SB message
    CFE_TBL_Handle_t   INTERFACE_tblHandle; ///< table handle
    gsInterfaceTable_t Tbl;                 ///< Table
    port_t gs;                              ///< groundstation port
    int receivingWP;                        ///< waypoint current being received
    int numWaypoints;                       ///< num total waypoints
    int numGeofences;                       ///< num total geofences
    int recvGeofIndex;                      ///< Index of geofence being received
    int waypointSeq;                        ///< received position waypoint
    int nextWaypointIndex;                  ///< Next waypoint index to goto.
    int hbeatFreqCount;                     ///< Counter for sending hbeat message
    mavlink_mission_item_t ReceivedMissionItems[MAX_WAYPOINTS];
    mavlink_mission_item_int_t ReceivedMissionItemsInt[MAX_WAYPOINTS];
    flightplan_t fpData;
    flightplan_t mgData;
    flightplan_t trajectory;
    argsCmd_t startMission;
    geofence_t gfData[MAX_GEOFENCES];
    uint16_t fenceVertices[MAX_GEOFENCES];
    object_t traffic;
    noArgsCmd_t resetIcarous;
    uint8_t currentApMode;
    uint8_t currentIcarousMode;
    param_t storedparams[PARAM_COUNT];
    uint32_t wptimer;
    uint32_t gftimer;
    uint32_t pmtimer;
    uint32_t tjtimer;
    uint32_t rcv_wp_seq;
    uint32_t rcv_gf_seq;
    bool fenceSent;
    bool paramSent;
    bool publishDefaultParams;
    callsign_t callsign;
}appdataIntGS_t;

/**
 * Entry point for app
 */
void gsInterface_AppMain(void);

/**
 * Initialize app properties
 */
void gsInterface_AppInit(void);

/**
 * Initialize app data
 */
void gsInterface_InitializeAppData(void);

/**
 * Clean up variables
 */
void gsInterface_AppCleanUp(void);

/**
 * Get mavlink message from ground station
 */
int GetMAVLinkMsgFromGS(void);

/**
 * Process mavlink message from groundstation and take action
 * @param message mavlink message
 */
void ProcessGSMessage(mavlink_message_t message);

/**
 * Process SB messages from pipes and take action
 */
void gsInterface_ProcessPacket(void);

/**
 * Send heartbeat message
 */
void gsSendHeartbeat();

/**
 * Send callsign
 */
void gsSendCallsign();


/**
 * Validate table data
 * @param *TblPtr pointer to table
 */
int32_t gsInterfaceTableValidationFunc(void *TblPtr);

appdataIntGS_t appdataIntGS;

/**
 *  Send all locally stored params over the SB
 */
void gsInterface_PublishParams();

void gsConvertMissionItemsToPlan(uint16_t  size, mavlink_mission_item_t items[],flightplan_t* fp);
void gsConvertMissionItemsIntToPlan(uint16_t  size, mavlink_mission_item_int_t items[],flightplan_t* fp);

uint16_t gsConvertPlanToMissionItems(flightplan_t* fp);

void gs_wpCallback(uint32_t timer);

void gs_gfCallback(uint32_t timer);

void gs_pmCallback(uint32_t timerId);

void gs_tjCallback(uint32_t timerId);

void gs_startTimer(uint32_t *timerID,void (*f)(uint32_t),char* name,uint32_t startTime,uint32_t intvl);

void gs_stopTimer(uint32_t *timerID);

void gsFragmentBands(bands_t *bands);

#define SendGSMsg(arg) writeMavlinkData(&appdataIntGS.gs,&arg)

#endif
