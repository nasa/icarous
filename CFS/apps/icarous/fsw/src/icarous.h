/*******************************************************************************
** File: icarous.h
**
** Purpose:
**
**
*******************************************************************************/

#ifndef _icarous_h_
#define _icarous_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "icarous_perfids.h"
#include "icarous_msgids.h"
#include "Icarous_msg.h"
#include "icarous_version.h"
#include "icarous_events.h"
#include "icarous_table.h"
#include "IcarousWrapper.h"

#define ICAROUS_PIPE_DEPTH    250

/**
 * \struct appdata_t
 *  Structure to contain global data
 */
typedef struct{
	CFE_SB_PipeId_t    ICAROUS_Pipe;          ///< Pipe descriptor
	CFE_SB_MsgPtr_t    ICAROUSMsgPtr;         ///< SB message pointer
	CFE_TBL_Handle_t   ICAROUS_tblHandle;     ///< Icarous table data
	struct icarous_t* ic;                     ///< opaque struct to interface with Cpp class
	icarous_table_t TblPtrCpy;                ///< Copy of current table data
	ArgsCmd_t cmd;                            ///< Commands from ICAROUS
	visbands_t bands;                         ///< Kinematic bands
}appdataIC_t;

/**
 * Main entry point for ICAROUS app
 */
void ICAROUS_AppMain(void);

/**
 * Initialization for app data
 */
void ICAROUS_AppInit(void);

/**
 * Process SB messages and take actions
 */
void ICAROUS_ProcessPacket(void);

/**
 * Get output from ICAROUS
 */
void ICAROUS_Output(void);

/**
 * Function to validate table data
 */
int32_t IcarousTableValidationFunc(void *TblPtr);

/// Gobal data
appdataIC_t appdataIC;

#endif /* _ICAROUS_app_h_ */
