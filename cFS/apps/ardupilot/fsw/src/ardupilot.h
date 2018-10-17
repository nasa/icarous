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

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "network_includes.h"
#include "ardupilot_events.h"
#include "msgdef/ardupilot_msg.h"
#include "msgdef/traffic_msg.h"
#include "msgids/ardupilot_msgids.h"
#include "msgids/traffic_msgids.h"
#include "msgids/scheduler_msgids.h"
#include "mavlink/ardupilotmega/mavlink.h"


#define TASK_1_ID         21    /// Task id for child task
#define TASK_2_ID         22    /// Task id for child task

#define TASK_1_STACK_SIZE 1024  ///< Stack size for task
#define TASK_2_STACK_SIZE 1024

#define TASK_1_PRIORITY   63    ///< Task priority
#define TASK_2_PRIORITY   63

#define BUFFER_LENGTH 1000  ///< Mavlink message receive buffer size

#define ARDUPILOT_PIPE_NAME "FLIGHTPLAN"
#define ARDUPILOT_PIPE_DEPTH 100

#define SCH_ARDUPILOT_PIPE1_NAME "SCH_ARDUPILOT1"
#define SCH_ARDUPILOT_PIPE2_NAME "SCH_ARDUPILOT2"

uint32 task_1_stack[TASK_1_STACK_SIZE]; ///< Byte array that will serve as stacks for for the child tasks
uint32 task_2_stack[TASK_2_STACK_SIZE];
uint32 task_1_id, task_2_id;

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
 * @enum PortType_t
 * @brief Port type
 */
typedef enum {
    SOCKET,  ///< enum value SOCKET
    SERIAL   ///< enum value SERIAL
} portType_e;

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
 *\struct port_t
 * @brief Structure to hold port attributes
 */
typedef struct{
    int id;                          ///< id
    portType_e portType;             ///< port type
    struct sockaddr_in target_addr;  ///< target address
    struct sockaddr_in self_addr;    ///< self address
    socklen_t recvlen;               ///< length of received host properties
    int sockId;                      ///< socket id
    int portin;                      ///< input socket
    int portout;                     ///< output socket
    char target[50];                 ///< target ip address/or name of serial port
    char recvbuffer[BUFFER_LENGTH];  ///< buffer for incoming data
    int baudrate;                    ///< baud rate only if a serial port
}port_t;

/**
 * @struct appdataInt_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    INTERFACE_Pipe;      ///< pipe variable
    CFE_SB_PipeId_t    SchInterface_Pipe1;  ///< pipe variable
    CFE_SB_PipeId_t    SchInterface_Pipe2;  ///< pipe variable
    CFE_SB_MsgPtr_t    INTERFACEMsgPtr;     ///< msg pointer to SB message
    CFE_SB_MsgPtr_t    Sch_MsgPtr1;         ///< msg pointer to SB message
    CFE_SB_MsgPtr_t    Sch_MsgPtr2;         ///< msg pointer to SB message
    CFE_TBL_Handle_t   INTERFACE_tblHandle; ///< table handle
    port_t ap;                              ///< autopilot port
    port_t gs;                              ///< groundstation port
    uint8_t runThreads;                     ///< thread active status
    int numWaypoints;                       ///< num total waypoints
    int waypointSeq;                        ///< received position waypoint
    int nextWaypointIndex;                  ///< Next waypoint index to goto.
    int* waypoint_type;                     ///< waypoint type description
    int* waypoint_index;                    ///< waypoint index (only positional waypoints)
    int foundUAV;                           ///< UAV communication alive
    uint32_t mutex_read;                    ///< mutex id
    uint32_t mutex_write;                   ///< mutex id
    bool startWPUplink;
    bool startWPDownlink;
    uint16_t  numUplinkWaypoints;
    uint16_t  numDownlinkWaypoints;
    uint16_t  downlinkRequestIndex;
    controlMode_e currentMode;
    mavlink_mission_item_t UplinkMissionItems[MAX_WAYPOINTS];
    mavlink_mission_item_t DownlinkMissionItems[MAX_WAYPOINTS];
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
 * Initialize a socket port
 * @param *prt pointer to port
 */
void InitializeSocketPort(port_t *prt);

/**
 * Initialize a serial port
 * @param *prt pointer to port
 */
int InitializeSerialPort(port_t* prt,bool should_block);


/**
 * Read from Ardupilot data stream and pass data to
 * groundstation and ICAROUS app
 */
void Task1(void);

/**
 * Read from groundstation data stream and pass to Ardupilot
 * and ICAROUS app
 */
void Task2(void);

/**
 * Read raw data from port
 * @param *prt pointer to port to read from
 */
int readPort(port_t *prt);

/**
 * Write mavlink message to a given port
 * @param *prt pointer to output port
 * @param *message pointer to mavlink message
 */
void writePort(port_t* prt,mavlink_message_t *message);

/**
 * Get mavlink message from Ardupilot
 */
int GetMAVLinkMsgFromAP(void);

/**
 * Get mavlink message from ground station
 */
int GetMAVLinkMsgFromGS(void);

/**
 * Process mavlink message from groundstation and take action
 * @param message mavlink message
 */
bool ProcessGSMessage(mavlink_message_t message);

/**
 * Process mavlink message from ardupilot and take action
 * @param message mavlink message
 */
void ProcessAPMessage(mavlink_message_t message);

/**
 * Process SB messages from pipes and take action
 */
void ARDUPILOT_ProcessPacket(void);

/**
 * Validate table data
 * @param *TblPtr pointer to table
 */
int32_t ArdupilotTableValidationFunc(void *TblPtr);

uint16_t ConvertPlanToMissionItems(flightplan_t* fp);

void ConvertMissionItemsToPlan(uint16_t size,mavlink_mission_item_t items[],flightplan_t *fp);

bool IntfServiceHandler(mavlink_message_t *message);

EXTERN appdataInt_t appdataInt;                ///< global variable containing app state
EXTERN flightplan_t fpdata;                    ///< waypoint message
EXTERN missionItemReached_t wpreached;         ///< mission item reached
EXTERN geofence_t gfdata;                      ///< geofence data
EXTERN argsCmd_t startMission;                 ///< start mission command
EXTERN noArgsCmd_t resetIcarous;               ///< reset icarous command
EXTERN object_t traffic;                       ///< traffic message
EXTERN position_t position;                    ///< position message
EXTERN cmdAck_t ack;                           ///< command acknowledge message

#endif /* _ardupilot_h_ */
