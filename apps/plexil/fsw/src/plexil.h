/**
 * @file plexil.h
 * @brief Function declaration and defintions of defines and data structures used in plexl app.
 */
#ifndef CFETOP_PLEXIL_H
#define CFETOP_PLEXIL_H

/// Required header files.
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"

#include <string.h>
#include <errno.h>
#include "PlexilWrapper.h"

#include "sch_msgids.h"
#include "plexil_table.h"

#define PLEXIL_MAJOR_VERSION 4
#define PLEXIL_MINOR_VERSION 0

/// Software bus properties
#define PLEXIL_PIPE_NAME "PLEXILPIPE"
#define PLEXIL_DATA_PIPE_NAME "PLEXILDATAPIPE"
#define PLEXIL_PIPE_DEPTH 100

// plexil App event defines
#define PLEXIL_RESERVED_EID              0
#define PLEXIL_STARTUP_INF_EID           1
#define PLEXIL_COMMAND_ERR_EID           2
#define PLEXIL_INVALID_MSGID_ERR_EID     3

#define CHECKNAME(MSG,STRING) !strcmp(MSG->name,STRING)

/**
 * @defgroup PLEXIL
 * @brief Plexil - plan execution cFS application
 * @ingroup APPLICATIONS
 *
 *
 * @details The plexil cFS application is a wrapper around the C++ PLEXIL application available at
 * (https://sourceforge.net/projects/plexil/). Plexil provides a framework for defining plans. Plans for Icarous can
 * be located under cFS/apps/plexil/plans. The plexil executive runs at a rate controlled by the cFS scheduler.
 * Plexil lookups and commands are passed through the custom cfs interface adapter to the cfs application. These lookups
 * and handles are then handled appropriately by the cfs applications. The plexil application is also subscribing to
 * various internal topics to get information such as aircraft position, velocity, flight plan, information from the
 * various monitors (traffic, geofence, trajectory) about conflicts. This information is inturn used by the various
 * plexil plans to make decisions.
 */

/**
 * @struct plexilAppData_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    PLEXIL_Pipe;       ///< pipe variable
    CFE_SB_MsgPtr_t    PLEXIL_MsgPtr;     ///< msg pointer to SB message
    CFE_SB_PipeId_t    DATA_Pipe;         ///< pipe variable
    CFE_SB_MsgPtr_t    DATA_MsgPtr;       ///< msg pointer to SB message
    CFE_TBL_Handle_t   PLEXIL_tblHandle;  ///< table handle
    struct plexilExec* exec;              ///< opaque structure for plexil executive
    struct plexilInterfaceAdapter* adap;  ///< opaque structure for plexil adapter interface
}plexilAppData_t;


EXTERN plexilAppData_t plexilAppData;                ///< global variable containing app state

/**
 * Entry point for app
 */
void PLEXIL_AppMain(void);

/**
 * Function to initialize app properties
 */
void PLEXIL_AppInit(void);

/**
 * Function to clean up variables
 */
void PLEXIL_AppCleanUp(void);

/**
 * Function to process SB messages
 * @param true if data packet
 */
void PLEXIL_ProcessPacket(bool);

/**
 * Function to run one interation of the PLEXIL executive loop
 */

void PLEXIL_Run();

/**
 * Function to validate plexil table parameters
 * @param TblPtr pointer to table
 * @return 0 for success.
 */

int32_t PlexilTableValidationFunc(void *TblPtr);

/**
 * Function to initialize custom data
 */
void PLEXIL_InitializeCustomData();

/**
 * Function to process custom command/lookup and data packets
 * @param true if data packet
 */
void PLEXIL_ProcessCustomPackets(bool);

/**
 * Function to distribute plexil lookups
 */
void PLEXIL_HandleCustomLookups(PlexilMsg *msg);

/**
 * Function to distribute plexil commands
 */
void PLEXIL_HandleCustomCommands(PlexilMsg *msg);


/**
 * Function to subscibe to mission specific messages.
 * If this cfs app is used outside the context of Icarous, this is where
 * custom subscriptions should go in.
 */
void PLEXIL_CustomSubscription(void);


#endif //CFETOP_PLEXIL_H
