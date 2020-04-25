/**
 * @file zmq_iface.h
 * @brief Function declaration, MACRO and data structure definitions for the zmq_iface application
 */

#ifndef ICAROUS_CFS_ZMQ_IFACE_H
#define ICAROUS_CFS_ZMQ_IFACE_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include <string.h>

#include "connection.h"

#define ZMQ_IFACE_STARTUP_INF_EID 0
#define ZMQ_IFACE_COMMAND_ERR_EID 1

#define ZMQ_IFACE_PIPE_DEPTH 10
#define ZMQ_IFACE_PIPE_NAME "ZMQ_IFACE_PIPE"
#define ZMQ_IFACE_MAJOR_VERSION 1
#define ZMQ_IFACE_MINOR_VERSION 0

/**
 * @defgroup ZMQ_IFACE
 * @brief Application to monitor traffic conflicts
 * @ingroup APPLICATIONS
 *
 * @details Description of this application
 *
 */


/**
 * @struct ZMQ_IFACE_AppData_t
 * @brief Structure to hold app data
 */
typedef struct {
    CFE_SB_PipeId_t    pipe;       ///< Pipe variable
    CFE_SB_MsgPtr_t    msgPtr;     ///< Msg pointer to SB message

    ZMQ_IFACE_Connection_t connection; ///< ZMQ connection configuration
} ZMQ_IFACE_AppData_t;

/**
 * App entry point
 */
void ZMQ_IFACE_AppMain(void);

/**
 * Initializes data structures and cfs related entities.
 */
void ZMQ_IFACE_AppInit(void);

/**
 * Deallocates memory
 */
void ZMQ_IFACE_AppCleanUp();

/**
 * Processes incoming software bus messages
 */
void ZMQ_IFACE_ProcessPacket();

#endif //ICAROUS_CFS_ZMQ_IFACE_H
