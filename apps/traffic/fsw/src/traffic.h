/**
 * @file traffic.h
 * @brief Function declaration, MACRO and data structure definitions for the traffic application
 */

#ifndef ICAROUS_CFS_TRAFFIC_H
#define ICAROUS_CFS_TRAFFIC_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"

#include <string.h>
#include <math.h>

#include "TrafficMonitor.h"

#include "Icarous.h"
#include "traffic_msgids.h"
#include "sch_msgids.h"

#include "traffic_msg.h"
#include "traffic_tbl.h"

#define TRAFFIC_STARTUP_INF_EID 0
#define TRAFFIC_COMMAND_ERR_EID 1
#define TRAFFIC_RECEIVED_INTRUDER_EID 2

#define TRAFFIC_PIPE_DEPTH 100
#define TRAFFIC_PIPE_NAME "TRAFFIC_PIPE"
#define TRAFFIC_MAJOR_VERSION 1
#define TRAFFIC_MINOR_VERSION 0

/**
 * @defgroup TRAFFIC_cFS_APP
 * @ingroup APPLICATIONS
 *
 * @details This application subscribes to the ownship information (position/velocity) and intruder information and computes
 * relevant metrics to identify imminent loss of separation. If a loss of separation is predicted, four different
 * resolutions are generated. Executing on of these resolutions well maeuver the ownship to remain well clear of any
 * intruding aircraft. Also provides a flag indicating the existence of conflict on a direct path from the current position
 * of the ownship to any of the mission waypoints. The internals of the traffic application makes use of the DAIDALUS
 * (https://github.com/nasa/WellClear) library. The input configuration file is specified in the input tables. Refer
 * to the DAIDALUS repository for more information on each parameter specified in the input configuration file. The
 * information published by this application is controlled by the scheduler.
 *
 */


/**
 * @struct TrajectoryAppData_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    Traffic_Pipe;       ///< Pipe variable
    CFE_SB_MsgPtr_t    Traffic_MsgPtr;     ///< Msg pointer to SB message
    void* tfMonitor;                       ///< Opaque pointer to TrafficMonitor
    CFE_TBL_Handle_t Traffic_tblHandle;    ///< Traffic table handle
    bands_t trackBands;                    ///< Track kinematic band data
    bands_t speedBands;                    ///< Ground speed kinematic band data
    bands_t vsBands;                       ///< Vertical speed kinematic band data
    bands_t altBands;                      ///< Altitude kinematic band data
    band_report_t bandReport;              ///< Band Report
    traffic_alerts_t tfAlerts;             ///< Traffic Alerts
    flightplan_t flightplan1;              ///< Mission flight plan information
    flightplan_t flightplan2;              ///< Secondary flight plan
    double position[3];                    ///< Current position as lat (deg), lon (deg) and alt (m)
    double velocity[3];                    ///< Current velocity as track (deg), ground speed (m/s) and vertical speed (m/s)
    double time;                           ///< Time from autopilot
    bool log;                              ///< log DAA output
    int numTraffic;                        ///< Number of traffic vehicles added
    bool return2fp1leg;                    ///< Return to flight plan 1 leg
    bool return2fp2leg;                    ///< Return to flight plan 2 leg
    uint32_t trafficSrc;                   ///< Traffic source selector
    bool updateDaaParams;                  ///< Update DAA params via SB messages
    traffic_parameters_t params;           ///< DAIDALUS parameters
}TrafficAppData_t;

/**
 * App entry point
 */
void TRAFFIC_AppMain(void);

/**
 * Initializes data structures and cfs related entities.
 */
void TRAFFIC_AppInit(void);

/**
 * Deallocates memory
 */
void TRAFFIC_AppCleanUp(void);

/**
 * Processes incoming software bus messages
 */
void TRAFFIC_ProcessPacket(void);

/**
 * Validates table parameters
 * @param TblPtr Pointer to table
 * @return  0 if success
 */
int32_t TrafficTableValidationFunc(void *TblPtr);

/**
 * @var Global variable to store all app related data
 */
TrafficAppData_t trafficAppData;

#endif //ICAROUS_CFS_TRAFFIC_H
