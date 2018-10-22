/**
 * @file radarinterface.h
 * @brief Definition of defines, structs and functions for geofence monitoring app
 *
 */

#ifndef _radarinterface_h_
#define _radarinterface_h_

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
#include "radarinterface_events.h"
#include "ardupilot_msg.h"
#include "ardupilot_msgids.h"


/// Mavlink includes
#include "mavlink/ardupilotmega/mavlink.h"

/// Mavlink message receive buffer
#define RDINTF_BUFFER_LENGTH 1000

/// Software bus properties
#define RADARINTERFACE_PIPE_NAME "RADARINTERFACE_PIPE"
#define RADARINTERFACE_PIPE_DEPTH 100


/**
 * enum radarInterface_PortType_t
 * Port type
 */
typedef enum {
    SOCKET,  ///< enum value SOCKET
    SERIAL   ///< enum value SERIAL
} radarInterface_PortType_t;

/**
 *\struct radarInterface_port_t
 * Structure to hold port attributes
 */
typedef struct{
    int id;                              ///< id
    radarInterface_PortType_t portType;  ///< port type
    struct sockaddr_in target_addr;      ///< target address
    struct sockaddr_in self_addr;        ///< self address
    socklen_t recvlen;                   ///< length of received host properties
    int sockId;                          ///< socket id
    int portin;                          ///< input socket
    int portout;                         ///< output socket
    char target[50];                     ///< target ip address/or name of serial port
    char recvbuffer[RDINTF_BUFFER_LENGTH];  ///< buffer for incoming data
    int baudrate;                        ///< baud rate only if a serial port
}radarInterface_port_t;

/**
 * \struct appdataRadar_t
 * Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    RADARINTERFACE_Pipe;     ///< pipe variable
    CFE_SB_MsgPtr_t    RADARINTERFACEMsgPtr;    ///< msg pointer to SB message
    radarInterface_port_t radarInterfacePort;
    double acPosition[3];
    double yaw;
    object_t traffic;
}appdataRadar_t;


/**
 * Entry point for app
 */
void RADARINTF_AppMain(void);

/**
 * Function to initialize app properties
 */
void RADARINTERFACE_AppInit(void);

/**
 * Function to clean up variables
 */
void RADARINTERFACE_AppCleanUp(void);

/**
 * Initialize a socket port
 * @param *prt pointer to port
 */
void RADARINTERFACE_InitializeSocketPort(radarInterface_port_t *prt);

/**
 * Read raw data from port
 * @param *prt pointer to port to read from
 */
int RADARINTERFACE_readPort(radarInterface_port_t *prt);

/**
 * Write mavlink message to a given port
 * @param *prt pointer to output port
 * @param *message pointer to mavlink message
 */
void RADARINTERFACE_writePort(radarInterface_port_t* prt,mavlink_message_t *message);

/**
 * Get mavlink message from radar application
 */
int RADARINTERFACE_GetMAVLinkMsg(void);


/**
 * Process mavlink message from radar and take action
 * @param message mavlink message
 */
void RADARINTERFACE_ProcessMavlinkMessage(mavlink_message_t message);


/**
 * Process SB messages from pipes and take action
 */
void RADARINTERFACE_ProcessSBMessage(void);



#endif /* _safe2ditch_app_h_ */
