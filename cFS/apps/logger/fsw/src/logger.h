/**
 * @file logger.h
 * @brief function declarations, definitions of macros, datastructures and global variables for the logging application
 */


#ifndef _logger_h_
#define _logger_h_

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "msgdef/ardupilot_msg.h"
#include "msgids/ardupilot_msgids.h"
#include "logger_table.h"
#include <stdio.h>


#define LOG_PIPE_APDATA_NAME "LOG_APDATA"
#define LOG_PIPE_COMMANDS_NAME "LOG_CMD"
#define LOG_PIPE_DEPTH 256

#define LOGGER_STARTUP_INF_EID 0
#define LOGGER_COMMAND_ERR_EID 1


/**
 * @defgroup LOGGER
 * @brief An application to log/playback cFS messages
 * @ingroup APPLICATIONS
 *
 * @details This application subscribes to data from autopilot, traffic information and also commands from
 * ground stations. These SB messages are recorded into individual log files under cFS/bin/ram/IClog. Setting
 * logRecord to true enables logging, false enables playback. Remove the application from startup if you don't
 * want to log data.
 *
 */


/**
 * @struct appdataInt_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    LOG_Commands_Pipe;       ///< pipe to capture commands to autopilot
    CFE_SB_PipeId_t    LOG_AutopilotData_Pipe;  ///< pipe to capture data from autopilot

    CFE_SB_MsgPtr_t    LOG_Command_MsgPtr;      ///< msg pointer to SB message (command data)
    CFE_SB_MsgPtr_t    LOG_APdata_MsgPtr;       ///< msg pointer to SB messages (autopilot data)

    CFE_TBL_Handle_t   LOGGER_tblHandle;        ///< table handle
    FILE * fpPos;                               ///< position log file
    FILE * fpCommands;                          ///< command log file
    FILE * fpFlightPlan;                        ///< flight plan log file
    FILE * fpGeofence;                          ///< geofence log file
    FILE * fpTraffic;                           ///< traffic log file
    FILE * fpStartMission;                      ///< start mission command
    FILE * fpCmdAck;                            ///< Command acknowledgement
    FILE * fpWpReached;                         ///< waypoint reached
    CFE_TIME_SysTime_t logRecStartTime;         ///< Start time recorded in log
    CFE_TIME_SysTime_t appStartTime;            ///< App start time
    bool record;                                ///< record log file
    char playbackTS[20];                        ///< play back time stamp
    char playbackLocation[100];                 ///< play back location
}appdataLog_t;


/**
 * Entry point for app
 */
void LOGGER_AppMain(void);

/**
 * Initialize app properties
 */
void LOGGER_AppInit(void);

/**
 * Clean up variables
 */
void LOGGER_AppCleanUp(void);

/**
 * Process incoming packets
 */
void LOGGER_ProcessPacket(CFE_SB_Msg_t* msg);

/**
 * Open log files for writing SB Messages
 */
void PrepareLogFiles();

/**
 * Record received SB messages to corresponding log files
 */
void RecordPackets();

/**
 * Open log files for reading
 */
void OpenLogFiles();

/**
 * Play back logged SB messages
 */
void PlaybackPackets();

/**
 * Drop the given SB message when the delLocal is greater than delMessage
 * @param delLocal time elapsed
 * @param msg logged SB message to publish
 * @return ture if sent/ false if not sent because of time constraint.
 */
bool PublishSBMessage(CFE_TIME_SysTime_t delLocal,CFE_SB_Msg_t* msg);

/**
 * Subscribe to all plexil inputs
 */
void SubscribeInputs();

/**
 * Validate table inputs
 */
int32_t LoggerTableValidationFunc(void *);


appdataLog_t appdataLog;  ///< app data

/**
 * global variables to store and send logged messages
 */
bool sentPos, havePos;
bool sentCommand, haveCommand;
bool sentFlightPlan, haveFlightPlan;
bool sentGeofence, haveGeofence;
bool sentTraffic, haveTraffic;
bool sentStartMission, haveStartMission;
bool sentCmdAck, haveCmdAck;
bool sentWPReached, haveWPReached;

position_t logPos;
geofence_t logGf;
argsCmd_t logCmd;
flightplan_t logFp;
object_t logTraffic;
argsCmd_t logStartMission;
cmdAck_t logCmdAck;
missionItemReached_t logWPReached;

#endif /* _ardupilot_h_ */
