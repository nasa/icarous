/*=======================================================================================
** File Name:  raft_msg.h
**
** Title:  Message Definition Header File for RAFT Application
**
** $Author:    raft
** $Revision: 1.1 $
** $Date:      2017-03-21
**
** Purpose:  To define RAFT's command and telemetry message defintions 
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2017-03-21 | raft | Build #: Code Started
**   2017-05-21 | JR   | Build #: CHREC-v0	
**	 2018-07-03	| CLM  | BUILD #: ICAROUS2-v0.0.1
**=====================================================================================*/
    
#ifndef _RAFT_MSG_H_
#define _RAFT_MSG_H_

/*
** Pragmas
*/

/*
** Include Files
*/



/*
** Local Defines
*/

/*
** RAFT command codes
*/
#define RAFT_NOOP_CC                 0
#define RAFT_RESET_CC                1
#define RAFT_FOLLOWER_CC			 2
#define RAFT_CANDIDATE_CC			 3
#define RAFT_LEADER_CC				 4

/*
** Local Structure Declarations
*/
typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              usCmdCnt;
    uint8              usCmdErrCnt;

    /* TODO:  Add declarations for additional housekeeping data here */

} RAFT_HkTlm_t;


#endif /* _RAFT_MSG_H_ */

/*=======================================================================================
** End of file raft_msg.h
**=====================================================================================*/
    
