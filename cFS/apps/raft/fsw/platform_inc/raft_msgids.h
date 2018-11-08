/*=======================================================================================
** File Name:  raft_msgids.h
**
** Title:  Message ID Header File for RAFT Application
**
** $Author:    raft
** $Revision: 1.1 $
** $Date:      2017-03-21
**
** Purpose:  This header file contains declartions and definitions of all RAFT's 
**           Message IDs.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2017-03-21 | raft | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _RAFT_MSGIDS_H_
#define _RAFT_MSGIDS_H_

/***** TODO:  These Message ID values are default and may need to be changed by the developer  *****/
#define RAFT_CMD_MID            	0x18C0
#define RAFT_SEND_HK_MID        	0x18C1
#define RAFT_WAKEUP_MID        		0x18D0
#define RAFT_OUT_DATA_MID        	0x18D1

#define RAFT_HK_TLM_MID		0x08BB

    


#endif /* _RAFT_MSGIDS_H_ */

/*=======================================================================================
** End of file raft_msgids.h
**=====================================================================================*/
    
