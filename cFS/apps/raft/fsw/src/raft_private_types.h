/*=======================================================================================
** File Name:  raft_private_types.h
**
** Title:  Type Header File for RAFT Application
**
** $Author:    raft
** $Revision: 1.1 $
** $Date:      2017-03-21
**
** Purpose:  This header file contains declarations and definitions of all RAFT's private
**           data structures and data types.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2017-03-21 | raft | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _RAFT_PRIVATE_TYPES_H_
#define _RAFT_PRIVATE_TYPES_H_

/*
** Pragmas
*/

/*
** Include Files
*/
#include "cfe.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
} RAFT_NoArgCmd_t;


typedef struct
{
    uint32  counter;

    /* TODO:  Add input data to this application here, such as raw data read from I/O
    **        devices or data subscribed from other apps' output data.
    */

} RAFT_InData_t;

typedef struct arrivalData{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];
    int aircraftID;
    int intersectionID;
    uint32_t earlyArrivalTime;
    uint32_t currentArrivalTime;
    uint32_t lateArrivalTime;
}arrivalData_t;

typedef struct
{
    uint8   ucTlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint32  uiCounter;
} RAFT_OutData_t;

/* TODO:  Add more private structure definitions here, if necessary. */

/*
** External Global Variables
*/

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/

#endif /* _RAFT_PRIVATE_TYPES_H_ */

/*=======================================================================================
** End of file raft_private_types.h
**=====================================================================================*/
    
