/**
 * @file FLARM.h
 * @brief function declarations, definitions of macros, datastructures and global variables for the FLARM application
 */
#ifndef _FLARM_h_
#define _FLARM_h_

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "UtilFunctions.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "network_includes.h"
#include "flarm_events.h"
#include "msgdef/ardupilot_msg.h"
#include "msgdef/traffic_msg.h"
#include "msgids/ardupilot_msgids.h"
#include "msgids/scheduler_msgids.h"
#include "msgids/traffic_msgids.h"
#include "port_lib.h"
#include "pflaa.h"

#define FLARM_PIPE_NAME "FLARM_PIPE"
#define FLARM_PIPE_DEPTH 100

#define SCH_FLARM_PIPE1_NAME "SCH_FLARM"


/**
 * @defgroup FLARM_cFS_APP
 * @ingroup APPLICATIONS
 *
 * @details cFS application to read data from a FLARM device
 *
 */

/**
 * @struct FlarmAppData_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    INTERFACE_Pipe;      ///< pipe variable
    CFE_SB_PipeId_t    SchInterface_Pipe;   ///< pipe variable
    CFE_SB_MsgPtr_t    INTERFACEMsgPtr;     ///< msg pointer to SB message
    CFE_SB_MsgPtr_t    Sch_MsgPtr;          ///< msg pointer to SB message
    CFE_TBL_Handle_t   INTERFACE_tblHandle; ///< table handle
    port_t fp;                              ///< flarm port
    position_t position;                    ///< position message
    double gpsOrigin[3];                    ///< gps Origin position
    double gpsEND[3];                       ///< END position
    attitude_t attitude;                    ///< attitude message
    object_t traffic;                       ///< traffic message
    missionItemReached_t wpReached;         ///< waypoint reached feedback message
    int currentWP;                          ///< autopilot's current waypoint
    flightplan_t flightplan;

    // You may need additional ports if data is obtained from other sensors
    // (e.g ADSB)
    //port_t sensor1;
}FlarmAppData_t;


/**
 * Entry point for app
 * @brief This is the main entry point for your application that will be used by cFS
 */
void FLARM_AppMain(void);

/**
 * Initialize app properties
 * @brief Initialize app data: set up pipes, subscribe to messages, read table, initialize data
 */
void FLARM_AppInit(void);

/**
 * Clean up variables
 */
void FLARM_AppCleanUp(void);

/**
 * Initialize ports to the autopilot
 * @brief Open ports to autopilot and intialize ports here
 */
void FLARM_InitializePorts();

/**
 * Process data from the autopilot
 * @brief Read data from the autopilot and construct software  bus messages
 */
void FLARM_ProcessData(void);

/**
 * Process data from the SB message
 * @brief Read data from software bus and send data to autopilot
 */
void FLARM_ProcessSBData(void);

/**
 * logs valid NMEA sentence
 * @brief Ensures read in serial data is full NMEA sentence 
 */
bool logSentence(char *recvbuffer, nmea_pflaa_s *pflaa);

/**
 * Obtains current time and date for filename 
 * @brief Ensures current time and date is captured 
 */
void getFileTimeandDate(char** Filename);
/**
 * Validate table data
 * @param *TblPtr pointer to table
 */
int32_t FLARMTableValidationFunc(void *TblPtr);

#endif /* _FLARM_h_ */
