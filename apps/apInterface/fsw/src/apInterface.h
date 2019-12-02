/**
 * @file apInterface.h
 * @brief function declarations, definitions of macros, datastructures and global variables for the apInterface application
 */
#ifndef _apInterface_h_
#define _apInterface_h_

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "network_includes.h"
#include "apInterface_events.h"
#include "Icarous_msg.h"
#include "traffic_msg.h"
#include "Icarous_msgids.h"
#include "sch_msgids.h"
#include "traffic_msgids.h"
#include "port_lib.h"

#define APINTERFACE_PIPE_NAME "APFLTPLAN"
#define APINTERFACE_PIPE_DEPTH 100

#define SCH_APINTERFACE_PIPE1_NAME "SCH_AUTOPILOT"

/**
 * @defgroup APINTERFACE
 * @brief A sample application to interface icarous with a autopilot system
 * @ingroup APPLICATIONS
 *
 * @details This is a sample application that interfaces with an autopilot.
 * This application should open a connection to the autopilot device. This can be any type of connection (serial/socket).
 * You could make use of the port_lib library provided as part of this code base to establish serial or socket (UDP) communication.
 * This app currently illustrates how one would open a port using the API provided in port_lib.
 * Make use of the messages provided by the scheduler to establish the rate at which you want functions to run. See
 * the @see ARDUPILOT for a concrete example of an interface.
 *
 */

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
    position_t position;                    ///< position message
    attitude_t attitude;                    ///< attitude message
    object_t traffic;                       ///< traffic message
    missionItemReached_t wpReached;         ///< waypoint reached feedback message
    int currentWP;                          ///< autopilot's current waypoint
    flightplan_t flightplan;

    // You may need additional ports if data is obtained from other sensors
    // (e.g ADSB)
    //port_t sensor1;
}appdataInt_t;


/**
 * Entry point for app
 * @brief This is the main entry point for your application that will be used by cFS
 */
void APINTERFACE_AppMain(void);

/**
 * Initialize app properties
 * @brief Initialize app data: set up pipes, subscribe to messages, read table, initialize data
 */
void APINTERFACE_AppInit(void);

/**
 * Clean up variables
 */
void APINTERFACE_AppCleanUp(void);

/**
 * Initialize ports to the autopilot
 * @brief Open ports to autopilot and intialize ports here
 */
void APINTERFACE_InitializeAPPorts();

/**
 * Process data from the autopilot
 * @brief Read data from the autopilot and construct software  bus messages
 */
void APINTERFACE_ProcessAPData(void);

/**
 * Process data from the SB message
 * @brief Read data from software bus and send data to autopilot
 */
void APINTERFACE_ProcessSBData(void);

/**
 * Process ADSB data
 * @brief Read ADSB data from ADSB transponder and publish traffic data on software bus.
 */
void APINTERFACE_GetADSBData();

/**
 * Validate table data
 * @param *TblPtr pointer to table
 */
int32_t ApInterfaceTableValidationFunc(void *TblPtr);

appdataInt_t appdataApIntf;

#endif /* _apInterface_h_ */
