/*******************************************************************************
** File: interface.h
**
** Purpose:
**   This file is main hdr file for the SAMPLE application.
**
**
*******************************************************************************/

#ifndef _interface_h_
#define _interface_h_

/// Required header files.
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "network_includes.h"
#include "interface_events.h"
#include "icarous_msg.h"
#include "icarous_msgids.h"

/// Mavlink includes
#include "mavlink/icarous/mavlink.h"


/// Defines required to specify stack properties
#define TASK_1_ID         1
#define TASK_1_STACK_SIZE 1024
#define TASK_1_PRIORITY   101

#define TASK_2_ID         2
#define TASK_2_STACK_SIZE 1024
#define TASK_2_PRIORITY   102

/// Mavlink message receive buffer
#define BUFFER_LENGTH 300

/// Software bus properties
#define INTERFACE_PIPE_NAME "FLIGHTPLAN"
#define INTERFACE_PIPE_DEPTH 32

/// Task related variables
uint32 task_1_stack[TASK_1_STACK_SIZE];
uint32 task_2_stack[TASK_2_STACK_SIZE];
uint32 task_1_id, task_2_id;

/**
 * enum PortType_t
 * Port type
 */
typedef enum {
  SOCKET,  ///< enum value SOCKET 
  SERIAL   ///< enum value SERIAL 
} PortType_t;

/**
 * enumeration for Ardupilot control modes
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
    LAND =          9,  ///< automatic landing with horizontal position control
    DRIFT =        11,  ///< semi-automous position, yaw and throttle control
    SPORT =        13,  ///< manual earth-frame angular rate control with manual throttle
    FLIP =         14,  ///< automatically flip the vehicle on the roll axis
    AUTOTUNE =     15,  ///< automatically tune the vehicle's roll and pitch gains
    POSHOLD =      16,  ///< automatic position hold with manual override, with automatic throttle
    BRAKE =        17,  ///< full-brake using inertial/GPS system, no pilot input
    THROW =        18,  ///< throw to launch mode using inertial/GPS system, no pilot input
    AVOID_ADSB =   19,  ///< automatic avoidance of obstacles in the macro scale - e.g. full-sized aircraft
    GUIDED_NOGPS = 20,  ///< guided mode but only accepts attitude and altitude
}control_mode_t;

/**
 *\struct port_t
 * Structure to hold port attributes
 */
typedef struct{
        int id;                          ///< id
	PortType_t portType;             ///< port type 
	struct sockaddr_in target_addr;  ///< target address 
	struct sockaddr_in self_addr;    ///< self address
 	socklen_t recvlen;               ///< length of received host properties
	int sockId;                      ///< socket id
	int portin;                      ///< input socket
	int portout;                     ///< output socket
	char target[50];                 ///< target ip address
	char recvbuffer[BUFFER_LENGTH];  ///< buffer for incoming data
	uint32_t mutex_id;               ///< mutex id
}port_t;

/**
 * \struct appdata_t
 * Structure to hold app data
 */
typedef struct{
  CFE_SB_PipeId_t    INTERFACE_Pipe;      ///< pipe variable
  CFE_SB_MsgPtr_t    INTERFACEMsgPtr;     ///< msg pointer to SB message
  CFE_TBL_Handle_t   INTERFACE_tblHandle; ///< table handle
  port_t ap;                              ///< autopilot port
  port_t gs;                              ///< groundstation port
  uint8_t runThreads;                     ///< thread active status
  int numWaypoints;                       ///< num total waypoints
  int* waypoint_type;                     ///< waypoint type description
  int foundUAV;                           ///< UAV communication alive
}appdataInt_t;


/**
 * Entry point for app
 */
void INTERFACE_AppMain(void);

/**
 * Function to initialize app properties
 */
void INTERFACE_AppInit(void);

/**
 * Function to clean up variables
 */
void INTERFACE_AppCleanUp(void);

/**
 * Initialize a socket port
 * @param *prt pointer to port
 */
void InitializeSocketPort(port_t *prt);

/**
 * Initialize a serial port
 * @param *prt pointer to port
 */
//void InitializeSerialPort(port_t* prt);


/**
 * Task1 
 * Read from Ardupilot data stream and pass data to 
 * groundstation and ICAROUS app
 */
void Task1(void);

/**
 * Task2
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
void ProcessGSMessage(mavlink_message_t message);

/**
 * Process mavlink message from ardupilot and take action
 * @param message mavlink message
 */
void ProcessAPMessage(mavlink_message_t message);

/**
 * Process SB messages from pipes and take action
 */
void INTERFACE_ProcessPacket(void);

/**
 * Function to validate table data
 * @param *TblPtr pointer to table
 */
int32_t InterfaceTableValidationFunc(void *TblPtr);


/**
 * Global variable declaration
 */

EXTERN appdataInt_t appdataInt;                ///< global variable containing app state
EXTERN waypoint_t wpdata;                      ///< waypoint message
EXTERN missionItemReached_t wpreached;         ///< mission item reached
EXTERN geofence_t gfdata;                      ///< geofence data
EXTERN ArgsCmd_t startMission;                 ///< start mission command
EXTERN NoArgsCmd_t resetIcarous;               ///< reset icarous command
EXTERN object_t traffic;                       ///< traffic message
EXTERN position_t position;                    ///< position message
EXTERN CmdAck_t ack;                           ///< command acknowledge message

#endif /* _interface_app_h_ */
