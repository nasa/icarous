/**
 * @file {{APP_NAME}}.h
 * @brief Function declaration, MACRO and data structure definitions for the {{APP_NAME}} application
 */

#ifndef ICAROUS_CFS_{{APP_NAMEU}}_H
#define ICAROUS_CFS_{{APP_NAMEU}}_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include <string.h>

#include "{{APP_NAME}}_tbl.h"

#define {{APP_NAMEU}}_STARTUP_INF_EID 0
#define {{APP_NAMEU}}_COMMAND_ERR_EID 1

#define {{APP_NAMEU}}_PIPE_DEPTH 30 
#define {{APP_NAMEU}}_PIPE_NAME "{{APP_NAMEU}}_PIPE"
#define {{APP_NAMEU}}_MAJOR_VERSION 1
#define {{APP_NAMEU}}_MINOR_VERSION 0

/**
 * @defgroup {{APP_NAMEU}}
 * @brief Application to monitor traffic conflicts
 * @ingroup APPLICATIONS
 *
 * @details Description of this application
 * 
 */


/**
 * @struct {{APP_NAME}}AppData_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    {{APP_NAME}}_Pipe;       ///< Pipe variable
    CFE_SB_MsgPtr_t    {{APP_NAME}}_MsgPtr;     ///< Msg pointer to SB message
    CFE_TBL_Handle_t   {{APP_NAME}}_tblHandle;    ///< Traffic table handle
}{{APP_NAME}}AppData_t;

/**
 * App entry point
 */
void {{APP_NAMEU}}_AppMain(void);

/**
 * Initializes data structures and cfs related entities.
 */
void {{APP_NAMEU}}_AppInit(void);

/**
 * Deallocates memory
 */
void {{APP_NAMEU}}_AppCleanUp();

/**
 * Processes incoming software bus messages
 */
void {{APP_NAMEU}}_ProcessPacket();

/**
 * Validates table parameters
 * @param TblPtr Pointer to table
 * @return  0 if success
 */
int32_t {{APP_NAME}}TableValidationFunc(void *TblPtr);

/**
 * @var Global variable to store all app related data
 */
{{APP_NAME}}AppData_t {{APP_NAME}}AppData;

#endif //ICAROUS_CFS_{{APP_NAMEU}}_H
