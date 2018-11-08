/*=======================================================================================
** File Name:  raft_private_ids.h
**
** Title:  ID Header File for RAFT Application
**
** $Author:    raft
** $Revision: 1.1 $
** $Date:      2017-03-21
**
** Purpose:  This header file contains declarations and definitions of RAFT's private IDs.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2017-03-21 | raft | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _RAFT_PRIVATE_IDS_H_
#define _RAFT_PRIVATE_IDS_H_

/*
** Pragmas
*/

/*
** Include Files
*/

/*
** Local Defines
*/

/* Event IDs */
#define RAFT_RESERVED_EID  0

#define RAFT_INF_EID        1
#define RAFT_INIT_INF_EID   2
#define RAFT_ILOAD_INF_EID  3
#define RAFT_CDS_INF_EID    4
#define RAFT_CMD_INF_EID    5

#define RAFT_ERR_EID         51
#define RAFT_INIT_ERR_EID    52
#define RAFT_ILOAD_ERR_EID   53
#define RAFT_CDS_ERR_EID     54
#define RAFT_CMD_ERR_EID     55
#define RAFT_PIPE_ERR_EID    56
#define RAFT_MSGID_ERR_EID   57
#define RAFT_MSGLEN_ERR_EID  58

#define RAFT_EVT_CNT  14

/*
** Local Structure Declarations
*/

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

#endif /* _RAFT_PRIVATE_IDS_H_ */

/*=======================================================================================
** End of file raft_private_ids.h
**=====================================================================================*/
    