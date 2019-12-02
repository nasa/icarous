/**
 * @file ardupilot.h
 * @brief function declarations, definitions of macros, datastructures and global variables for the arudpilot application
 */
#ifndef _ardupilot_h_
#define _ardupilot_h_

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <paramdef.h>
#include <network_includes.h>
#include <ardupilot_events.h>
#include <Icarous_msg.h>
#include <Icarous_msgids.h>
#include <traffic_msg.h>
#include <traffic_msgids.h>
#include <guidance_msg.h>
#include <guidance_msgids.h>
#include <geofence_msg.h>
#include <geofence_msgids.h>
#include <trajectory_msg.h>
#include <trajectory_msgids.h>
#include <tracking_msg.h>
#include <tracking_msgids.h>
#include <sch_msgids.h>
#include <mavlink/ardupilotmega/mavlink.h>
#include <port_lib.h>

#define ARDUPILOT_PIPE_NAME "FLIGHTPLAN"
#define ARDUPILOT_PIPE_DEPTH 100

#define SCH_ARDUPILOT_PIPE1_NAME "SCH_ARDUPILOT"

/**
 * @defgroup ARDUPILOT
 * @brief An application to interface cFS with an ardupilot autopilot system
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
 * @enum controlMode_e
 * @brief enumeration for Ardupilot control modes
 */
typedef enum{
    STABILIZE =     0,  ///< manual airframe angle with manual throttle
    ACRO =          1,  ///< manual body-frame angular rate with manual throttle
    ALT_HOLD =      2,  ///< manual airframe angle with automatic throttle
    AUTO =          3,  ///< fully automatic waypoint control using mission commands
    GUIDED =        4,  ///< fully automatic fly to coordinate or fly at velocity/direction using GCS immediate commands
    LOITER =        5,  ///< automatic horizontal acceleration with automatic throttle
    RTL =           6,  ///< automatic return to launching point
    CIRCLE =        7,  ///< automatic circular flight with automatic throttle
    DRIFT =        11,  ///< semi-automous position, yaw and throttle control
    SPORT =        13,  ///< manual earth-frame angular rate control with manual throttle
    FLIP =         14,  ///< automatically flip the vehicle on the roll axis
    AUTOTUNE =     15,  ///< automatically tune the vehicle's roll and pitch gains
    POSHOLD =      16,  ///< automatic position hold with manual override, with automatic throttle
    BRAKE =        17,  ///< full-brake using inertial/GPS system, no pilot input
    THROW =        18,  ///< throw to launch mode using inertial/GPS system, no pilot input
    AVOID_ADSB =   19,  ///< automatic avoidance of obstacles in the macro scale - e.g. full-sized aircraft
    GUIDED_NOGPS = 20,  ///< guided mode but only accepts attitude and altitude
}controlMode_e;

/**
 * @struct appdataInt_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    INTERFACE_Pipe;      ///< pipe variable
    CFE_SB_PipeId_t    SchInterface_Pipe;   ///< pipe variable
    CFE_SB_MsgPtr_t    INTERFACEMsgPtr;     ///< msg pointer to SB message
    CFE_SB_MsgPtr_t    Sch_MsgPtr;          ///< msg pointer to SB message
    CFE_TBL_Handle_t   INTERFACE_tblHandle; ///< table handle
    port_t ap;                              ///< autopilot port
    uint8_t runThreads;                     ///< thread active status
    int numWaypoints;                       ///< num total waypoints
    int waypointSeq;                        ///< received position waypoint
    int nextWaypointIndex;                  ///< Next waypoint index to goto.
    int numGeofences;                       ///< Number of geofences
    int* waypoint_type;                     ///< waypoint type description
    int waypoint_index[MAX_WAYPOINTS];      ///< waypoint index (only positional waypoints)
    int foundUAV;                           ///< UAV communication alive
    uint32_t mutex_read;                    ///< mutex id
    uint32_t mutex_write;                   ///< mutex id
    bool startWPUplink;
    bool startWPDownlink;
    int recvGeofIndex;                      ///< Index of geofence being received
    geofence_t gfData[MAX_GEOFENCES];
    uint16_t fenceVertices[MAX_GEOFENCES];
    uint16_t  numUplinkWaypoints;
    uint16_t  numDownlinkWaypoints;
    uint16_t  downlinkRequestIndex;
    controlMode_e currentAPMode;
    uint8_t icarousMode;
    mavlink_mission_item_t UplinkMissionItems[MAX_WAYPOINTS];
    mavlink_mission_item_t DownlinkMissionItems[MAX_WAYPOINTS];
    bool startMission;
    bool restartMission;
    bool fpread;
    mavlink_mission_item_t ReceivedMissionItems[MAX_WAYPOINTS];
    uint32_t home_latitude;                 ///< vehicle home position latitude in degE7
    uint32_t home_longitude;                ///< vehicle home position longitude in degE7
    uint32_t home_altitude;                 ///< vehicle home position altitude in mm
    bool takeoff;

    int receivingWP;                        ///< waypoint current being received
    param_t storedparams[PARAM_COUNT];
    uint32_t wptimer;
    uint32_t gftimer;
    uint32_t pmtimer;
    uint32_t tjtimer;
    uint32_t rcv_wp_seq;
    uint32_t rcv_gf_seq;
    flightplan_t trajectory;
    bool fenceSent;
    bool paramSent;


    uint8_t icRcChannel;
    uint32_t pwmStart;
    uint32_t pwmReset;
}appdataInt_t;


/**
 * Entry point for app
 */
void ARDUPILOT_AppMain(void);

/**
 * Initialize app properties
 */
void ARDUPILOT_AppInit(void);

/**
 * Clean up variables
 */
void ARDUPILOT_AppCleanUp(void);

/**
 * Read from Ardupilot data stream and pass data to
 * groundstation and ICAROUS app
 */
void Task1(void);

/**
 * Get mavlink message from Ardupilot
 */
int GetMAVLinkMsgFromAP(void);

/**
 * Process mavlink message from ardupilot and take action
 * @param message mavlink message
 */
void ProcessAPMessage(mavlink_message_t message);

/**
 * Process SB messages from pipes and take action
 */
void ARDUPILOT_ProcessPacket(void);


void apSendHeartbeat();

void ap_gfCallback(uint32_t timer);

void ap_pmCallback(uint32_t timerId);

void ap_tjCallback(uint32_t timerId);


void ap_startTimer(uint32_t *timerID,void (*f)(uint32_t),char* name,uint32_t startTime,uint32_t intvl);

void ap_stopTimer(uint32_t *timerID);

void apInterface_PublishParams();

/**
 * Validate table data
 * @param *TblPtr pointer to table
 */
int32_t ArdupilotTableValidationFunc(void *TblPtr);

uint16_t apConvertPlanToMissionItems(flightplan_t* fp);

void apConvertMissionItemsToPlan(uint16_t size,mavlink_mission_item_t items[],flightplan_t *fp);

bool IntfServiceHandler(mavlink_message_t *message);

EXTERN appdataInt_t appdataInt;                ///< global variable containing app state
EXTERN flightplan_t fpdata;                    ///< waypoint message
EXTERN missionItemReached_t wpreached;         ///< mission item reached
EXTERN geofence_t gfdata;                      ///< geofence data
EXTERN argsCmd_t startMission;                 ///< start mission command
EXTERN noArgsCmd_t resetIcarous;               ///< reset icarous command
EXTERN object_t traffic;                       ///< traffic message
EXTERN position_t position;                    ///< position message
EXTERN local_position_t local_position;        ///< local position message
EXTERN attitude_t attitude;                    ///< attitude message
EXTERN battery_status_t battery_status;        ///< battery status message
EXTERN cmdAck_t ack;                           ///< command acknowledge message
EXTERN vfrhud_t vfrhud;                        ///< vfr hud data
EXTERN rc_channels_t rc_channels;              ///< rc channels

#define apNextParam appdataInt.storedparams[i].value;i++; 

#endif /* _ardupilot_h_ */
