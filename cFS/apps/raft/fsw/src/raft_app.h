/*=======================================================================================
** File Name:  raft_app.h
**
** Title:  Header File for RAFT Application
**
** $Author:    raft
** $Revision: 1.1 $
** $Date:      2017-03-21
**
** Purpose:  To define RAFT's internal macros, data types, global variables and
**           function prototypes
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2017-03-21 | raft | Build #: Code Started
**   2017-05-21 | JR   | Build #: CHREC-v0	
**	 2018-07-03	| CLM  | BUILD #: ICAROUS2-v0.0.1
**=====================================================================================*/

#ifndef _RAFT_APP_H_
#define _RAFT_APP_H_

/*
** Pragmas
*/

/*
** Include Files
*/
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>

#define DEBUG

#include "raft_platform_cfg.h"
#include "raft_mission_cfg.h"
#include "raft_private_ids.h"
#include "raft_private_types.h"
#include "raft_perfids.h"
#include "raft_msgids.h"
#include "raft_msg.h"



/*
** Local Defines
*/
#define RAFT_TIMEOUT_MSEC    1000

#define RAFT_HB_ID 0x18C9
#define RAFT_RV_ID 0x18DF
#define RAFT_VOTE_ID 0x18D1
#define RAFT_HBACK_ID 0x18D2

#define RAFT_DUP_LEADER -2

#define RAFT_LOGPUSH_ID 0x18CA 						//ICAROUS
#define RAFT_LOGACC_ID 0x18DA 						//ICAROUS
//#define RAFT_LOGPUSH_ID 0x18CB 						//ICAROUS
//#define RAFT_LOGUDACK_ID 0x18DB 					//ICAROUS

#define INTERSECTION_MAX 5 							//ICAROUS

#define MERGER_RTD_MID 0x0140						//MERGER 

/** Raft states **/
#define RAFT_ERR -1
#define RAFT_FOLLOWER 0
#define RAFT_CANDIDATE 1
#define RAFT_LEADER 2

#define MS_BETWEEN_ACKS 25
/*
** Local Structure Declarations
*/



typedef struct TlmLog 						//ICAROUS
{
	int32 R;
	int32 T;
	int32 D;
} TlmLog_t;

typedef struct RAFT_TlmMsg 			 		//ICAROUS
{
    CFE_SB_TlmHdr_t hdr;
    int32 term;
    int32 node_id;
    int32 state;
    int32 nodes_in_swarm;
	TlmLog_t log[INTERSECTION_MAX];			//ICAROUS
	int32 index;
} Raft_TlmMsg_t;

typedef struct RAFT_TlmAckMsg 				//ICAROUS
{
	CFE_SB_TlmHdr_t hdr;
	int32 term;
	int32 node_id;
	int32 dest_node;
	TlmLog_t log[INTERSECTION_MAX];			//ICAROUS
} Raft_TlmAck_t;


typedef struct Raft_Status {
  int32 id;
  int32 term;
  	int32 index;
  int32 state;
  int32 nodes_in_swarm;
} Raft_Status_t;


typedef struct Raft_HBMsg {
    CFE_SB_TlmHdr_t hdr;
    int32 term;
    int32 node_id;
    int32 state;
    int32 nodes_in_swarm;
	int32 log[INTERSECTION_MAX];			//ICAROUS
} Raft_HBMsg_t;

typedef struct Raft_HBAckMsg {
	CFE_SB_TlmHdr_t hdr;
	int32 term;
	int32 node_id;
	int32 dest_node;
	int32 log[INTERSECTION_MAX];			//ICAROUS
} Raft_HBAck_t;

typedef struct Raft_RequestVote			
{
    CFE_SB_TlmHdr_t hdr;
    int32 term;
  	int32 index;
    int32 node_id;
    int32 raft_state;
} Raft_RequestVote_t;

typedef struct Raft_LogAcc {
    CFE_SB_TlmHdr_t hdr;
    int32 term;
    int32 node_id;
    int32 index;
    int32 entries;
    int32 numNodes;
    arrivalData_t log_ud_acc[INTERSECTION_MAX];            //ICAROUS
} log_ud_acc_t;

typedef struct CLOCK					//RAFT
	{
		int32 status;
		int log_cycle;
	} CLOCK;
/*
typedef struct arrivalData{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];
    int aircraftID;
    int intersectionID;
    double earlyArrivalTime;
    double currentArrivalTime;
    double lateArrivalTime;
} arrivalData_t;*/


	/* MERGER APP PIPE */
/*
typedef struct{
    CFE_SB_PipeId_t    Merger_Pipe;       ///< Pipe variable
    CFE_SB_MsgPtr_t    Merger_MsgPtr;     ///< Pointer to SB message
    CFE_TBL_Handle_t   Merger_tblHandle;  ///< Table handle
}mergerAppData_t;
*/

// MERGER_RTD_MID (defined above, already -CLM)

typedef CLOCK	SCH_CLOCK;					//RAFT
typedef Raft_TlmMsg_t RaftLog_push_t;		//ICAROUS
typedef Raft_TlmAck_t RaftLog_update_t;		//ICAROUS

typedef Raft_RequestVote_t Raft_VoteResponse_t;

typedef struct
{
    /* CFE Event table */
    CFE_EVS_BinFilter_t  EventTbl[RAFT_EVT_CNT];

    /* CFE scheduling pipe */
    CFE_SB_PipeId_t  SchPipeId;
    uint16           usSchPipeDepth;
    char             cSchPipeName[OS_MAX_API_NAME];

    /* CFE command pipe */
    CFE_SB_PipeId_t  CmdPipeId;
    uint16           usCmdPipeDepth;
    char             cCmdPipeName[OS_MAX_API_NAME];

    /* CFE telemetry pipe */
    CFE_SB_PipeId_t  TlmPipeId;
    uint16           usTlmPipeDepth;
    char             cTlmPipeName[OS_MAX_API_NAME];

    /* Raft heartbeat pipe */
    CFE_SB_PipeId_t HBPipeId;
    uint16          HBPipeDepth;
    char            HBPipeName[OS_MAX_API_NAME];

    /* Raft heartbeat ack pipe */
    CFE_SB_PipeId_t HBACKPipeId;
    uint16          HBACKPipeDepth;
    char            HBACKPipeName[OS_MAX_API_NAME];

    /* Raft RequestVote pipe */
    CFE_SB_PipeId_t RVPipeId;
    uint16          RVPipeDepth;
    char            RVPipeName[OS_MAX_API_NAME];

    /* Raft VoteResponse pipe */
    CFE_SB_PipeId_t VotePipeId;
    uint16          VotePipeDepth;
    char            VotePipeName[OS_MAX_API_NAME];

    /* Raft LogPush pipe */
    CFE_SB_PipeId_t LOGPUSHPipeId;
    uint16          LOGPUSHPipeDepth;
    char            LOGPUSHPipeName[OS_MAX_API_NAME];


	/* Merger OUTPUT DATA pipe*/								//MERGER
	CFE_SB_PipeId_t MERGERPipeId;
    uint16          MERGERPipeDepth;
    char            MERGERPipeName[OS_MAX_API_NAME];


	/* LOG ACC pipe*/											//RAFT
	CFE_SB_PipeId_t LOGACCPipeId;
    uint16          LOGACCPipeDepth;
    char            LOGACCPipeName[OS_MAX_API_NAME];

    /* Raft LogUpdate pipe */
/*
    CFE_SB_PipeId_t LOGUDPipeId;
    uint16          LOGUDPipeDepth;
    char            LOGUDPipeName[OS_MAX_API_NAME];
*/

    /* Task-related */
    uint32  uiRunStatus;

    /* Input data - from I/O devices or subscribed from other apps' output data.
       Data structure should be defined in raft/fsw/src/raft_private_types.h */
    RAFT_InData_t   InData;

    /* Output data - to be published at the end of a Wakeup cycle.
       Data structure should be defined in raft/fsw/src/raft_private_types.h */
    RAFT_OutData_t  OutData;

    /* Housekeeping telemetry - for downlink only.
       Data structure should be defined in raft/fsw/src/raft_msg.h */
    RAFT_HkTlm_t  HkTlm;

    /* TODO:  Add declarations for additional private data here */

    int32 node_id;
    int is_coordinator; // 1 node is coordinator
} RAFT_AppData_t;

/*
** External Global Variables
*/

/*
** Global Variables
*/
 RAFT_AppData_t  g_RAFT_AppData;
 Raft_Status_t Raft_Status;
/*
** Local Variables
*/

/*
** Local Function Prototypes
*/
int32  RAFT_InitApp(void);
int32  RAFT_InitEvent(void);
int32  RAFT_InitData(void);
int32  RAFT_InitPipe(void);

void  RAFT_AppMain(void);

void  RAFT_CleanupCallback(void);

int32 RAFT_RcvMsg(int32 iBlocking);

void  RAFT_ProcessNewData(void);
void  RAFT_ProcessNewCmds(void);
void  RAFT_ProcessNewAppCmds(CFE_SB_Msg_t*);

void  RAFT_ReportHousekeeping(void);
void  RAFT_SendOutData(void);

boolean  RAFT_VerifyCmdLength(CFE_SB_Msg_t*, uint16);

/**************************/
/** Raft Initializations **/
/**************************/


/** RAFT HB FUNCTIONS **/

 int32 listen_for_heartbeats();
 int32 listen_for_request_vote();
 int32 listen_for_vote();
 void send_heartbeat();
 void send_vote();
 void send_request_vote();


/** RAFT LOG FUNCTIONS **/

 int32 Raft_Clock(int*);							//RAFT

 int32 RAFT_LogIndex(int*);				//MERGER
 int32 push_log(int*);						//ICAROUS
 int32 log_accumlate(int*);				//ICAROUS
 void listen_for_updates(int*);			//ICAROUS

/** Raft state handler functions **/
 void set_node_id();
 int32 RAFT_follower();
 int32 RAFT_candidate();
 int32 RAFT_leader();
 void follower_timeout(uint32 timer_id);
 void candidate_timeout(uint32 timer_id);


//void RAFT_LogFunction(int);						//RAFT
/*  void raft_log_accumulate(); */
 
// int32 push_log(int);							//ICAROUS
// int32 log_accumlate(int);					//ICAROUS
// void listen_for_updates(int*);					//ICAROUS
// //  void raft_log_update();						//ICAROUS - debug
//int32 RAFT_LogIndex(int log_cycle);				//MERGER


#endif /* _RAFT_APP_H_ */

/*=======================================================================================
** End of file raft_app.h
**=====================================================================================*/
