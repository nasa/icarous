/************************************************************************
** File:
**
** Purpose: 
**  Define RAFT App Event messages
**
** Notes:
**
** $Log: raft_events.h  
** Revision 1.1 2018/0/10 
** Initial revision
** 
** CLM - 7-10-2018
**
*************************************************************************/
#ifndef _raft_events_h_
#define _raft_events_h_

/*****************************************************************************/

/* Event message ID's */
#define RAFT_EVM_RESERVED            0

#define RAFT_INIT_INF_EID            1   
#define RAFT_CRCMDPIPE_ERR_EID       2
#define RAFT_TLMOUTENA_INF_EID       3 
#define RAFT_SUBSCRIBE_ERR_EID       4
#define RAFT_TLMPIPE_ERR_EID         5
#define RAFT_MSGID_ERR_EID           6

/******************************************************************************/

#endif /* _to_lab_events_h_ */
