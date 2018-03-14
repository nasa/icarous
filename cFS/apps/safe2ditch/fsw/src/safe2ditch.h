/*******************************************************************************
** File: interface.h
**
** Purpose:
**   This file is main hdr file for the SAMPLE application.
**
**
*******************************************************************************/

#ifndef _safe2ditch_h_
#define _safe2ditch_h_

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
#include "safe2ditch_events.h"
#include "Icarous_msg.h"
#include "Plexil_msg.h"
#include "msgids/msgids.h"


/// Mavlink includes
#include "mavlink/ardupilotmega/mavlink.h"

/// Mavlink message receive buffer
#define S2D_BUFFER_LENGTH 1000

/// Software bus properties
#define SAFE2DITCH_PIPE_NAME "SAFE2DITCH_PIPE"
#define SAFE2DITCH_PIPE_DEPTH 100


/**
 * enum Safe2Ditch_PortType_t
 * Port type
 */
typedef enum {
    SOCKET,  ///< enum value SOCKET
    SERIAL   ///< enum value SERIAL
} s2d_PortType_t;

/**
 *\struct s2d_ort_t
 * Structure to hold port attributes
 */
typedef struct{
    int id;                              ///< id
    s2d_PortType_t portType;             ///< port type
    struct sockaddr_in target_addr;      ///< target address
    struct sockaddr_in self_addr;        ///< self address
    socklen_t recvlen;                   ///< length of received host properties
    int sockId;                          ///< socket id
    int portin;                          ///< input socket
    int portout;                         ///< output socket
    char target[50];                     ///< target ip address/or name of serial port
    char recvbuffer[S2D_BUFFER_LENGTH];  ///< buffer for incoming data
    int baudrate;                        ///< baud rate only if a serial port
}s2d_port_t;

/**
 * \struct appdata_t
 * Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    SAFE2DITCH_Pipe;     ///< pipe variable
    CFE_SB_MsgPtr_t    SAFE2DITCHMsgPtr;    ///< msg pointer to SB message
    s2d_port_t s2dport;
    boolean s2dInitialized;
    boolean ditchRequested;
    service_t ditchSite;
}appdataS2D_t;


/**
 * Entry point for app
 */
void SAFE2DITCH_AppMain(void);

/**
 * Function to initialize app properties
 */
void SAFE2DITCH_AppInit(void);

/**
 * Function to clean up variables
 */
void SAFE2DITCH_AppCleanUp(void);

/**
 * Initialize a socket port
 * @param *prt pointer to port
 */
void s2d_InitializeSocketPort(s2d_port_t *prt);

/**
 * Read raw data from port
 * @param *prt pointer to port to read from
 */
int s2d_readPort(s2d_port_t *prt);

/**
 * Write mavlink message to a given port
 * @param *prt pointer to output port
 * @param *message pointer to mavlink message
 */
void s2d_writePort(s2d_port_t* prt,mavlink_message_t *message);

/**
 * Get mavlink message from safe2ditch
 */
int GetMAVLinkMsg(void);


/**
 * Process mavlink message from saf2ditch and take action
 * @param message mavlink message
 */
void ProcessMavlinkMessage(mavlink_message_t message);


/**
 * Process SB messages from pipes and take action
 */
void ProcessSBMessage(void);



#endif /* _safe2ditch_app_h_ */
