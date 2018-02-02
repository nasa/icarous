/*******************************************************************************
** File: interface.h
**
** Purpose:
**   This file is main hdr file for the SAMPLE application.
**
**
*******************************************************************************/

#ifndef _dds_interface_h_
#define _dds_interface_h_

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
#include "dds_interface_events.h"
#include "Icarous_msg.h"
#include "msgids/msgids.h"


/// Mavlink includes
#include "mavlink/ardupilotmega/mavlink.h"




/// Software bus properties
#define DDS_INTERFACE_PIPE_NAME "DDS_INTERFACE"
#define DDS_INTERFACE_PIPE_DEPTH 100

/**
 * \struct appdata_t
 * Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    DDS_INTERFACE_Pipe;      ///< pipe variable
    CFE_SB_MsgPtr_t    DDS_INTERFACEMsgPtr;     ///< msg pointer to SB message
}appdataDDSInt_t;


/**
 * Entry point for app
 */
void DDS_INTERFACE_AppMain(void);

/**
 * Function to initialize app properties
 */
void DDS_INTERFACE_AppInit(void);

/**
 * Function to clean up variables
 */
void DDS_INTERFACE_AppCleanUp(void);


void DDS_INTERFACE_ProcessPacket(void);

EXTERN appdataDDSInt_t appdataDDSInt;

#endif /* _dds_interface_app_h_ */
