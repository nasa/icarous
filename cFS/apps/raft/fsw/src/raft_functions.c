/*=======================================================================================
** File Name:  raft_functions.c
**
** Title:  Function Definitions for RAFT Application
**
** $Author:    RAFT
** $Revision: 1.1 $
** $Date:      2017-03-21
**
** Purpose:  This source file contains all necessary function definitions to run RAFT
**           application.
**
** Functions Defined:
**    Function X - Brief purpose of function X
**    Function Y - Brief purpose of function Y
**    Function Z - Brief purpose of function Z
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to all functions in the file.
**    2. List the external source(s) and event(s) that can cause the funcs in this
**       file to execute.
**    3. List known limitations that apply to the funcs in this file.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2017-03-21 | raft | Build #: Code Started
**   2017-05-21 | JR   | Build #: CHREC-v0	
**	 2018-07-03	| CLM  | BUILD #: ICAROUS2-v0.0.1
**=====================================================================================*/
/*
** Include Files
*/
#define EXTERN extern
#include <string.h>

#include "cfe.h"

#include "raft_platform_cfg.h"
#include "raft_mission_cfg.h"
#include "raft_app.h"
#include "raft_events.h"


/*******************************/
/*** RAFT APP VOTER FUNCTIONS **/
/*******************************/

int32 listen_for_vote() 
{
	int status = CFE_SUCCESS;
	int term = -1;
	CFE_SB_MsgPtr_t received_message;
	Raft_VoteResponse_t rcv_vote;
	status = CFE_SB_RcvMsg(&received_message, g_RAFT_AppData.VotePipeId, CFE_SB_POLL);

	if (status==CFE_SB_NO_MESSAGE) 
		return term;

	if (status!=CFE_SUCCESS) 
		printf("RAFT Node %i failed to read from vote response pipe.\n", Raft_Status.id);
	
	memcpy(&rcv_vote, received_message, sizeof(rcv_vote));

	if (rcv_vote.term > Raft_Status.term && rcv_vote.raft_state == RAFT_LEADER ) 
		term = RAFT_DUP_LEADER;
	else if (rcv_vote.term > Raft_Status.term)
		term = -1;
	else
		term = rcv_vote.term;
#ifdef DEBUG
	printf("\033[32;1mRAFT CANDIDATE Node %i: received vote response from Node %i. Term: %i\033[0m\n", Raft_Status.id, rcv_vote.node_id, rcv_vote.term);
#endif

  return term;
}

/** On HB recieve function returns CANDIDATE term. If HB PIPE is empty, -1 is returned. ** This is not an error condition. **/


void send_heartbeat() 
{
	int status = CFE_SUCCESS;
	Raft_HBMsg_t heartbeat_message;
	CFE_SB_InitMsg((CFE_SB_MsgPtr_t)&heartbeat_message, RAFT_HB_ID, sizeof(heartbeat_message), FALSE);
	heartbeat_message.node_id = Raft_Status.id;
	heartbeat_message.term = Raft_Status.term;
	heartbeat_message.state = Raft_Status.state;
	heartbeat_message.nodes_in_swarm = Raft_Status.nodes_in_swarm;
	heartbeat_message.log[0] = 1; 									// ICAROUS
		CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &heartbeat_message);
		status = CFE_SB_SendMsg((CFE_SB_MsgPtr_t)&heartbeat_message);

#ifdef DEBUG
	if (status!=CFE_SUCCESS) 
		printf("RAFT Node %i failed to send heartbeat.\n", Raft_Status.id);
	else
		printf("\033[32;1m\nRAFT LEADER Node %i: Sending heartbeat message. Node %i term: %i \033[0m\n", Raft_Status.id, Raft_Status.id, Raft_Status.term);
#endif

}

int32 listen_for_heartbeats() 
{

#ifdef DEBUG
	OS_printf ("\n\n HB LISTEN CALL\n\n"); //DEBUG
#endif

	int status = CFE_SUCCESS;
	int term = -1;
	CFE_SB_MsgPtr_t received_message;
	Raft_HBMsg_t rcv_hb;
	status = CFE_SB_RcvMsg(&received_message, g_RAFT_AppData.HBPipeId, CFE_SB_POLL);

	if (status == CFE_SB_NO_MESSAGE) 
		return term;

	if (status!=CFE_SUCCESS) 
		printf("RAFT Node %i failed to read from heartbeat pipe. Status: %i\n", Raft_Status.id, status);

	memcpy(&rcv_hb, received_message, sizeof(rcv_hb));
	Raft_Status.nodes_in_swarm = rcv_hb.nodes_in_swarm;
  /** Handle node reading old heartbearts from itself **/
	if (rcv_hb.node_id == Raft_Status.id)
		return term;
	
	/************************/
 	/** Send heartbeat ACK **/
	/************************/

	Raft_HBAck_t ack_msg;
	CFE_SB_InitMsg((CFE_SB_MsgPtr_t)&ack_msg, RAFT_HBACK_ID, sizeof(ack_msg), FALSE);
	ack_msg.node_id = Raft_Status.id;
	ack_msg.term = Raft_Status.term;
	ack_msg.dest_node = rcv_hb.node_id;
	ack_msg.log[0] = 1; 												// ICAROUS
	
	CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &ack_msg);
	status = CFE_SB_SendMsg((CFE_SB_MsgPtr_t)&ack_msg);
#ifdef DEBUG
	OS_printf("\033[32;1mRAFT FOLLOWER Node %i: received heartbeat message from Node %i. Node %i term: %i\033[0m\n", Raft_Status.id, rcv_hb.node_id, Raft_Status.id, Raft_Status.term);
#endif
	if (status!=CFE_SUCCESS) 
		OS_printf("RAFT Node %i failed to send heartbeat acknowledge.\n", Raft_Status.id);

	return Raft_Status.term;
}

void parse_heartbeat_acks() 
{
  int status = CFE_SUCCESS;
  int term = -1;
  int milliseconds_elapsed = 0;
  CFE_SB_MsgPtr_t received_message;
  Raft_HBAck_t rcv_ack;
  Raft_Status.nodes_in_swarm = 1;
	int HB_store[INTERSECTION_MAX];	

	for (int i=0; i<=INTERSECTION_MAX; i++)
	{
		HB_store[i]=0;
	}

	while (milliseconds_elapsed <= 1000) 
	{
    	status = CFE_SB_RcvMsg(&received_message, g_RAFT_AppData.HBACKPipeId, CFE_SB_POLL);
    	if (status!=CFE_SB_NO_MESSAGE) 
		{
			memcpy(&rcv_ack, received_message, sizeof(rcv_ack));

			if (rcv_ack.dest_node == Raft_Status.id) //Raft_Status.nodes_in_swarm++;
					HB_store[rcv_ack.node_id]=1;
		}
	
    	if (OS_TaskDelay(MS_BETWEEN_ACKS) != OS_SUCCESS)
			OS_printf("Raft Node %i OS_TaskDelay FAILED.\n", Raft_Status.id);
    	else 
			milliseconds_elapsed += MS_BETWEEN_ACKS;
	}
	
	for (int i=0; i<=INTERSECTION_MAX; i++)
	{
		if (HB_store[i]==1)
			Raft_Status.nodes_in_swarm++;
	}

#ifdef DEBUG
	OS_printf("\033[32;1mRAFT LEADER Node %i received %i heartbeat acknowledgements. Node %i term: %i\033[0m\n",
        Raft_Status.id, Raft_Status.nodes_in_swarm-1, Raft_Status.id, Raft_Status.term);
#endif
}

void send_request_vote() 
{
	int status = CFE_SUCCESS;
	Raft_RequestVote_t request_vote;
	CFE_SB_InitMsg((CFE_SB_MsgPtr_t)&request_vote, RAFT_RV_ID, sizeof(request_vote), FALSE);
	request_vote.node_id = 	Raft_Status.id;
	request_vote.term = 	Raft_Status.term;
	request_vote.index =	Raft_Status.index;
	CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &request_vote);
	status = CFE_SB_SendMsg((CFE_SB_MsgPtr_t)&request_vote);

#ifdef DEBUG
	if (status!=CFE_SUCCESS) 
		OS_printf("RAFT Node %i failed to send vote request.\n", Raft_Status.id);
	else
		OS_printf("\033[32;1mRAFT CANDIDATE Node %i: Sending vote request message\033[0m\n",  Raft_Status.id);
#endif

}

 int32 listen_for_request_vote() 
{

#ifdef DEBUG
	OS_printf ("\n\n RV LISTEN CALL\n\n"); //DEBUG
#endif

  int status = CFE_SUCCESS;
  int term = -1;
  CFE_SB_MsgPtr_t received_message;
  Raft_RequestVote_t rcv_rv;
  status = CFE_SB_RcvMsg(&received_message, g_RAFT_AppData.RVPipeId, CFE_SB_POLL);
 
	if (status == CFE_SB_NO_MESSAGE) 
		return term;
	if (status != CFE_SUCCESS) 
		OS_printf("RAFT Node %i failed to read from request vote pipe.\n", Raft_Status.id);

	memcpy(&rcv_rv, received_message, sizeof(rcv_rv));
	if (rcv_rv.node_id != Raft_Status.id)
	{
		term = rcv_rv.term;
#ifdef DEBUG
		OS_printf("\033[32;1mRAFT Node %i: received request vote message from Node %i. Term: %i\033[0m\n", Raft_Status.id, rcv_rv.node_id, rcv_rv.term);
#endif
	}

  return term;
}

 void send_vote() {
	int status = CFE_SUCCESS;
	Raft_VoteResponse_t vote_message;
	CFE_SB_InitMsg((CFE_SB_MsgPtr_t)&vote_message, RAFT_VOTE_ID, sizeof(vote_message), FALSE);
	vote_message.node_id = 	Raft_Status.id;
	vote_message.term = 	Raft_Status.term;
	vote_message.index = 	Raft_Status.index;
	vote_message.raft_state = Raft_Status.state;

	CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &vote_message);
	status = CFE_SB_SendMsg((CFE_SB_MsgPtr_t)&vote_message);
#ifdef DEBUG
	if (status!=CFE_SUCCESS) 
		printf("RAFT Node %i failed to send vote response.\n", Raft_Status.id);
	else
		printf("\033[32;1mRAFT Node %i: Sending vote response message\033[0m\n",  Raft_Status.id);
#endif
}



/*******************************/
/** RAFT APP LOGGER FUNCTIONS **/
/*******************************/


void listen_for_updates(int *log_cycle) 				  								//ICAROUS
{

	int status = CFE_SUCCESS;
	int term = -1;
	CFE_SB_MsgPtr_t received_message;
	int i;
	int sid;
	int logid;
	
	arrivalData_t log_ud;									//MERGER typed:private_types
	arrivalData_t ZERO;
	arrivalData_t log_ud_buff[INTERSECTION_MAX];								//MERGER
	arrivalData_t log_enter_f;
	//buffered
	log_ud_acc_t	log_acc;
	CFE_SB_InitMsg((CFE_SB_MsgPtr_t)&log_acc, RAFT_LOGACC_ID, sizeof(log_acc), TRUE);

	// time period SCH WID
	ZERO.aircraftID = -1;
	ZERO.earlyArrivalTime = 0.0;
	ZERO.currentArrivalTime = 0.0;
	ZERO.lateArrivalTime = 0.0;

	//memset(&log_ud_buff, 0, sizeof(log_ud)*INTERSECTION_MAX);
	
	for (i=0; i<INTERSECTION_MAX; i++)
	{
			//logid = i;	 					// SINGLE CRAFT LOG UPDATE
			log_ud_buff[i] = ZERO;			// USE LOGID to INDEX UPDATES into BUFFER
	}
	
	for (i=0; i<INTERSECTION_MAX; i++)
	{
		status = CFE_SB_RcvMsg(&received_message, g_RAFT_AppData.MERGERPipeId, CFE_SB_POLL);
		if (status == CFE_SB_NO_MESSAGE) 
		{
#ifdef DEBUG
			OS_printf("\nNO LOG UPDATES IN PIPE!\n");
#endif
			//logid = i;	 					// SINGLE CRAFT LOG UPDATE
			//log_ud_buff[i] = ZERO;			// USE LOGID to INDEX UPDATES into BUFFER
		}
		else if (status!=CFE_SUCCESS) 
		{
			printf("\nRAFT Node %i failed to read log updates. Status: %i\n", Raft_Status.id, status);
		}
		else 	
		{
#ifdef DEBUG
		OS_printf ("\nRECIEVING - UPDATE LISTENER\n");
#endif
		memcpy(&log_ud, received_message, sizeof(log_ud));	// INSTEAD OF SENDING TO ANOTHER APP, DO THIS FOR NOW:

		OS_printf("\033[35;1m\nLOG UPDATE: %d, %lu, %lu, %lu\033[0m\n\n",log_ud.aircraftID, log_ud.earlyArrivalTime,log_ud.currentArrivalTime,log_ud.lateArrivalTime);
	 
		logid = log_ud.aircraftID; //% INTERSECTION_MAX; // SINGLE CRAFT LOG UPDATE
		//log_ud_buff[logid] = log_ud;			// USE LOGID to INDEX UPDATES into BUFFER
#ifdef DEBUG
		OS_printf ("\n%i\n", logid);
#endif	
		memcpy(log_ud_buff+logid,&log_ud,sizeof(log_ud)); // RETURN FROM "i" debugging to earlier, correct LOGID

		}
	}

#ifdef DEBUG
	if(status == CFE_SUCCESS){
		printf("received data\n");
	}
#endif

	log_acc.term = 		Raft_Status.term;
	log_acc.node_id = 	Raft_Status.id;
	log_acc.index = 	log_cycle;
	log_acc.numNodes=	Raft_Status.nodes_in_swarm;	
	log_acc.entries = 	0;		// PARSE PLACE FOR SWEE

	memcpy(log_acc.log_ud_acc, log_ud_buff, sizeof(log_ud_buff));

	for (sid = 0; sid < INTERSECTION_MAX; sid++)
	{
	log_enter_f= log_acc.log_ud_acc[sid]; // ISSUE/WRITE LOG ENTRY BY INDIVIDUAL UPDATE
		if (log_enter_f.aircraftID != -1)	// ENTRY FOR SWEE
			log_acc.entries = log_acc.entries + 1; // ENTRY FOR SWEE
#ifdef DEBUG
		OS_printf("\n");
		printf("\033[35;1mRAFT LEADER Node %i:\033[0m\n", Raft_Status.id);
		printf("\033[35;1m  TOTAL UPDATES: %i\033[0m\n", log_acc.entries);
		printf("\033[35;1m  LOG TO INDEX %i:\033[0m\n", log_acc.index);
		printf("\033[35;1m  LOG FROM: %i\033[0m\n", log_enter_f.aircraftID);
		printf("\033[35;1m  LOG UPDATE: %lu, %lu, %lu\033[0m\n\n", log_enter_f.earlyArrivalTime,log_enter_f.currentArrivalTime,log_enter_f.lateArrivalTime);
#endif
	}

	CFE_SB_TimeStampMsg((CFE_SB_Msg_t*) &log_acc);
	status = CFE_SB_SendMsg((CFE_SB_MsgPtr_t)&log_acc);
	if (status==CFE_SUCCESS) 		
	{
#ifdef DEBUG
		OS_printf ("\033[35;1m\n\tRAFT LOG ENTRY %i SYNC:\t%i.\033[0m\n\n", log_cycle, Raft_Status.id); // ICAROUS
#endif
	}

#ifdef DEBUG
	else
		OS_printf("\n\n\nTOTALLY BOGUS!\n\n\n");
#endif

}

int32 log_accumulate(int *log_cycle)				  	//RAFT
{	
	int status;
	CFE_SB_MsgPtr_t received_message;
	int index = -1 ; 					// DEBUG with -1
	int sid;
	int logid;
	log_ud_acc_t	log_acc;			// RAFT_LOGACC_ID						//RAFT
	arrivalData_t log_update;

	status = CFE_SB_RcvMsg(&received_message, g_RAFT_AppData.LOGACCPipeId, CFE_SB_POLL);

	if (status == CFE_SUCCESS) 	//if (status!=CFE_SB_NO_MESSAGE) 
	{
		memcpy(&log_acc, received_message, sizeof(log_acc));
		if (log_cycle <= log_acc.index)
		{
			if (Raft_Status.state != RAFT_LEADER)	
				log_cycle = log_acc.index;
#ifdef DEBUG
			OS_printf("\033[34;1m\n\tRAFT Log Entry %i. Term: %i. Node %i - Leader.\033[0m\n\n\n", log_acc.index, log_acc.term, log_acc.node_id);
#endif

			for (int sid = 0; sid < INTERSECTION_MAX; sid++) // REC LOG ENTRY BY UPDATE
			{
				//memcpy(&log_update, &log_acc.log_ud_acc[sid], sizeof(log_update)); 
				log_update = log_acc.log_ud_acc[sid];
#ifdef DEBUG
				printf("\033[34;1mRAFT FOLLOWER Node %i:\033[0m\n", Raft_Status.id);
				printf("\033[34;1m  LOG FROM: %i\033[0m\n", log_update.aircraftID);
				printf("\033[34;1m  LOG ENTRY: %lu, %lu, %lu\033[0m\n\n", 	log_acc.log_ud_acc[sid].earlyArrivalTime, log_acc.log_ud_acc[sid].currentArrivalTime, log_acc.log_ud_acc[sid].lateArrivalTime);
#endif
			}
		}	
		else 					// DEBUG - Why does this eat an old message, sometimes?
		{
#ifdef DEBUG
			OS_printf("\033[54;1m\n\n\t\tWHOA, NELLI! GET NEW INDEX\033[0m\n\n");
			OS_printf("INTERNAL: %i\n",log_cycle);
			OS_printf("RECIEVED: %i\n",log_acc.index);
#endif
			if (Raft_Status.state != RAFT_LEADER)	
				log_cycle = log_acc.index;

#ifdef DEBUG
			OS_printf("\033[34;1m\n\tRAFT Log Entry %i. Term: %i. Node %i - Leader.\033[0m\n\n\n", log_acc.index, log_acc.term, log_acc.node_id);
#endif

			for (int sid = 0; sid < INTERSECTION_MAX; sid++) // REC LOG ENTRY BY UPDATE
			{
				//memcpy(&log_update, &log_acc.log_ud_acc[sid], sizeof(log_update)); 
				log_update = log_acc.log_ud_acc[sid];
#ifdef DEBUG
				printf("\033[34;1mRAFT FOLLOWER Node %i:\033[0m\n", Raft_Status.id);
				printf("\033[34;1m  LOG FROM: %i\033[0m\n", log_update.aircraftID);
				printf("\033[34;1m  LOG ENTRY: %lu, %lu, %lu\033[0m\n\n", 	log_acc.log_ud_acc[sid].earlyArrivalTime, log_acc.log_ud_acc[sid].currentArrivalTime, log_acc.log_ud_acc[sid].lateArrivalTime);
#endif
			}
		}
	}
	else if (status!=CFE_SUCCESS) 
	{
#ifdef DEBUG
		OS_printf("\033[54;1m\n\nI'm Sorry, Dave. I'm afraid I can't do that.\033[0m\n\n");
#endif
		log_cycle = log_cycle;
	}

	else 
	{
#ifdef DEBUG
		OS_printf("\033[54;1m\n\nWhere's Waldo?\033[0m\n\n");
#endif
		log_cycle = log_cycle;
	}
#ifdef DEBUG
	OS_printf ("\033[34;1m\n\tRAFT LOG ENTRY %i ACK:\t%i.\033[0m\n\n", log_cycle, Raft_Status.id);	// ICAROUS
#endif

	return log_cycle;
}

int32 push_log(int *log_cycle)										//ICAROUS
{
	int status = CFE_SUCCESS;
	RaftLog_push_t log_push;	
	CFE_SB_InitMsg((CFE_SB_MsgPtr_t)&log_push, RAFT_LOGPUSH_ID, sizeof(log_push), FALSE);	//MERGER
	log_push.node_id = 			Raft_Status.id;
	log_push.term =				Raft_Status.term;
	log_push.index =			log_cycle;
	int32 log_i = log_cycle;

	CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &log_push);
	status = CFE_SB_SendMsg((CFE_SB_MsgPtr_t)&log_push);
		// INSTEAD OF SENDING TO ANOTHER APP, DO THIS FOR NOW:
	if (status!=CFE_SUCCESS)
	{
		OS_printf("\033[31;1m\t\tRAFT LEADER %i failed to send gathered log, index %i.\n", Raft_Status.id, log_i);

		return log_i;
	}	

	log_i=log_accumulate(log_i);		// FOLLOW/LEAD: SUB to LEAD Push_Log() PUBs 
#ifdef DEBUG
	printf("\033[35;1m\tRAFT LEADER %i: Sending LOG ENTRY %i to storage\033[0m\n",  Raft_Status.id, log_i);
#endif

	return log_i;
}


