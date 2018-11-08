/*=======================================================================================
** File Name:  raft_app.c
**
** Title:  Function Definitions and States for RAFT
** Author: Chris Manderion
**=====================================================================================*/
/*
** Pragmas
*/
/*
** Include Files
*/

#define EXTERN
#include <string.h>

#include "cfe.h"

#include "raft_platform_cfg.h"
#include "raft_mission_cfg.h"
#include "raft_app.h"
#include "raft_events.h"
#include <stdbool.h>

//#include "raft_private_types.h"

#define DEBUG

unsigned int election_timeout = 0;
bool followerTimerSet = FALSE;


int32 RAFT_InitEvent()
{
    int32  iStatus=CFE_SUCCESS;

    /* Create the event table */
    memset((void*)g_RAFT_AppData.EventTbl, 0x00, sizeof(g_RAFT_AppData.EventTbl));

    g_RAFT_AppData.EventTbl[0].EventID = RAFT_RESERVED_EID;
    g_RAFT_AppData.EventTbl[1].EventID = RAFT_INF_EID;
    g_RAFT_AppData.EventTbl[2].EventID = RAFT_INIT_INF_EID;
    g_RAFT_AppData.EventTbl[3].EventID = RAFT_ILOAD_INF_EID;
    g_RAFT_AppData.EventTbl[4].EventID = RAFT_CDS_INF_EID;
    g_RAFT_AppData.EventTbl[5].EventID = RAFT_CMD_INF_EID;

    g_RAFT_AppData.EventTbl[ 6].EventID = RAFT_ERR_EID;
    g_RAFT_AppData.EventTbl[ 7].EventID = RAFT_INIT_ERR_EID;
    g_RAFT_AppData.EventTbl[ 8].EventID = RAFT_ILOAD_ERR_EID;
    g_RAFT_AppData.EventTbl[ 9].EventID = RAFT_CDS_ERR_EID;
    g_RAFT_AppData.EventTbl[10].EventID = RAFT_CMD_ERR_EID;
    g_RAFT_AppData.EventTbl[11].EventID = RAFT_PIPE_ERR_EID;
    g_RAFT_AppData.EventTbl[12].EventID = RAFT_MSGID_ERR_EID;
    g_RAFT_AppData.EventTbl[13].EventID = RAFT_MSGLEN_ERR_EID;

    /* Register the table with CFE */
    iStatus = CFE_EVS_Register(g_RAFT_AppData.EventTbl,
                               RAFT_EVT_CNT, CFE_EVS_BINARY_FILTER);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("RAFT - Failed to register with EVS (0x%08X)\n", iStatus);
    }

    return (iStatus);
}

int32 RAFT_RcvMsg(int32 iBlocking)
{
    int32           iStatus=CFE_SUCCESS;
    CFE_SB_Msg_t*   MsgPtr=NULL;
    CFE_SB_MsgId_t  MsgId;;

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(RAFT_MAIN_TASK_PERF_ID);

    /* Wait for WakeUp messages from scheduler */
//    iStatus = CFE_SB_RcvMsg(&MsgPtr, g_RAFT_AppData.SchPipeId, iBlocking);

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(RAFT_MAIN_TASK_PERF_ID);

    if (iStatus == CFE_SUCCESS)
    {
        MsgId = CFE_SB_GetMsgId(MsgPtr);
        switch (MsgId)
	{
            case RAFT_WAKEUP_MID:
                RAFT_ProcessNewCmds();
                RAFT_ProcessNewData();
                /* TODO:  Add more code here to handle other things when app wakes up */

                /* The last thing to do at the end of this Wakeup cycle should be to
                   automatically publish new output. vv */

                RAFT_SendOutData();
                break;

            default:
                CFE_EVS_SendEvent(RAFT_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "RAFT - Recvd invalid SCH msgId (0x%08X)", MsgId);
        }
    }
    else if (iStatus == CFE_SB_NO_MESSAGE)
    {
        /* If there's no incoming message, you can do something here, or nothing */
		OS_printf ("\n\nNO MESSAGE\n\n");
    }
    else
    {
        /* This is an example of exiting on an error.
        ** NB: a SB read error is not always going to result in an app quitting.
        */
        CFE_EVS_SendEvent(RAFT_PIPE_ERR_EID, CFE_EVS_ERROR,
			  "RAFT: SB pipe read error (0x%08X), app will exit", iStatus);
        g_RAFT_AppData.uiRunStatus= CFE_ES_APP_ERROR;
    }

    return (iStatus);
}


int32 RAFT_InitApp()
{
    int32  iStatus=CFE_SUCCESS;
    set_node_id();
    Raft_Status.nodes_in_swarm = 1; // count self in num nodes

    g_RAFT_AppData.uiRunStatus = CFE_ES_APP_RUN;

    iStatus = CFE_ES_RegisterApp();
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("RAFT - Failed to register the app (0x%08X)\n", iStatus);
        goto RAFT_InitApp_Exit_Tag;
    }

    if ((RAFT_InitEvent() != CFE_SUCCESS) ||
        (RAFT_InitPipe() != CFE_SUCCESS) ||
        (RAFT_InitData() != CFE_SUCCESS))
    {
        iStatus = -1;
        goto RAFT_InitApp_Exit_Tag;
    }

	/*****************************************/
    /* Install the cleanup callback */
	/*****************************************/

    OS_TaskInstallDeleteHandler((void*)&RAFT_CleanupCallback);

RAFT_InitApp_Exit_Tag:
    if (iStatus == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(RAFT_INIT_INF_EID, CFE_EVS_INFORMATION, "RAFT - Application initialized");
    }
    else
    {
        CFE_ES_WriteToSysLog("RAFT - Application failed to initialize\n");
    }

    return (iStatus);
}

void RAFT_CleanupCallback()
{
    /* TODO:  Add code to cleanup memory and other cleanup here */
}

void RAFT_AppMain()
{
    int32 iStatus = CFE_SUCCESS;
    /* Register the application with Executive Services */
    CFE_ES_RegisterApp();

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(RAFT_MAIN_TASK_PERF_ID);

    /* Perform application initializations */
    if (RAFT_InitApp() != CFE_SUCCESS)
    {
        g_RAFT_AppData.uiRunStatus = CFE_ES_APP_ERROR;
    } 
	else 
	{
        /* Do not perform performance monitoring on startup sync */
        CFE_ES_PerfLogExit(RAFT_MAIN_TASK_PERF_ID);
        CFE_ES_WaitForStartupSync(RAFT_TIMEOUT_MSEC);
        CFE_ES_PerfLogEntry(RAFT_MAIN_TASK_PERF_ID);
    }

    /* Application main loop */
	Raft_Status.term = 0;
	Raft_Status.index = 0;
    
	while (CFE_ES_RunLoop(&g_RAFT_AppData.uiRunStatus) == TRUE)
    {

	    switch(Raft_Status.state) 
		{
	    	case RAFT_FOLLOWER:
				//OS_printf ("\n\nI'm in the while FOLLOWER STATE HANDLER!\n\n");
				RAFT_follower();
   				break;
	        case RAFT_CANDIDATE:
				//OS_printf ("\n\nI'm in the while CANDIDATE STATE HANDLER!\n\n");
   				RAFT_candidate();
   				break;
   		    case RAFT_LEADER:
				//OS_printf ("\n\nI'm in the while LEADER STATE HANDLER!\n\n");
   			    RAFT_leader();
   				break;
    	   	case RAFT_ERR:
   			    g_RAFT_AppData.uiRunStatus = 0;
   			    break;
      	}
		
    }

    /* Stop Performance Log entry */
  //  CFE_ES_PerfLogExit(RAFT_MAIN_TASK_PERF_ID);

    /* Exit the application */
    CFE_ES_ExitApp(g_RAFT_AppData.uiRunStatus);
}

/*******************************/
/** INITIALIZE RAFT APP CLOCK **/
/*******************************/

 int32 Raft_Clock(int *log_cycle)
{
	//int32 iBlocking = CFE_SB_PEND_FOREVER; // CFE_SB_PEND_FOREVER; // CFE_SB_POLL;
	//int	iStatus = RAFT_RcvMsg(iBlocking);
	int log_index;

	//if (iStatus == CFE_SUCCESS)
	if (CFE_ES_RunLoop(&g_RAFT_AppData.uiRunStatus) == TRUE)
	if (Raft_Status.state != RAFT_CANDIDATE)
	{
		log_index = log_cycle;

#ifdef DEBUG
		OS_printf("\033[32;1m\n\t\tEntering RAFT Cycle: %i\033[0m\n", log_index);
#endif

		log_index = 		RAFT_LogIndex(log_cycle);
		Raft_Status.index = log_index;
		log_index++;

		return log_index;

	}
	else
	{
#ifdef DEBUG
		OS_printf ("\033[31;1m \n\n\t\t CANDIDATES MAY NOT ALTER LOGS\033[0m\t\t\n\n");
#endif
		//log_index++; "\033[31;1m    \033[0m"
		return -1; 									// DEBUG
	}	
}

/*******************************/
/** INITIALIZE RAFT APP LOGGER */
/*******************************/

int32 RAFT_LogIndex(int *log_cycle) // PROCS EACH TIME ON SCH WAKEUP_MID RCV_MSG DURING RAFT_RCVMSG LOOP -- Should know STATE
{
	//int log_i;		//DEBUG
	switch (Raft_Status.state)
	{
		case RAFT_LEADER:
			listen_for_updates(log_cycle);										// LEAD: SUB all Autopilot Clients Log_Update()	
#ifdef DEBUG
			OS_printf("\033[52;1m\tRAFT-LOG CHECK INDEX: %i \033[0m\n",log_cycle);// ICAROUS
#endif
				//log_i=push_log(log_cycle);										// LEAD: PUB to FOLLOWs SUBbing Log_Accumulate()
				// Log_Update(): Autopilots NEED to SYNC timing to this same period
			log_cycle=push_log(log_cycle);										//LEAD: PUB to FOLLOWs SUBed Log_Accumulate()
			//OS_printf("\n\nLAST LOG CHECK, INDEX = %i", log_cycle);			//DEBUG
				//log_cycle++;
				//OS_printf("\nLOG ENTRY APPENDED, INDEX: %i\n", log_i);			//DEBUG
			//OS_printf("\nLOG ENTRY APPENDED, INDEX: %i\n", log_cycle);		//DEBUG
			Raft_Status.index = log_cycle;										// FOR CONTINUITY AND ACCURACY
			return log_cycle;

		case RAFT_FOLLOWER:
			//OS_printf("\n\n\t\tMADE IT FOLLOWER LOG ACC CHECK\n\n");			//DEBUG
			log_cycle=log_accumulate(log_cycle);// FOLLOW/LEAD: SUB to LEAD Push_Log() PUBs 
				// Log_Update(): Autopilots NEED to SYNC timing to this same period
				// It may be necessary to have Autopilot push here, anyway, for timing 
			//OS_printf("FOLLOWER: LOG INDEX ACCEPTED: %i", log_cycle);			//DEBUG
			Raft_Status.index = log_cycle;										// FOR CONTINUITY AND ACCURACY
			return log_cycle;
	
		case RAFT_CANDIDATE:
#ifdef DEBUG
			OS_printf ("CANDIDATE STATUS: CANNOT APPEND - UPDATING AUX STORAGE");
#endif
				// Could be removed, here to preserve completeness
			return log_cycle;

		default: 		
#ifdef DEBUG
			OS_printf ("RAFT Logging Error Occurred - Notify Administrator");
#endif
			log_cycle++;
			return log_cycle;
				// break; || return RAFT_ERR;
	}
}


/*******************************/
/**** INITIALIZE RAFT NODE  ****/
/*******************************/

 void set_node_id() 
{
	int id = CFE_PSP_GetSpacecraftId();
	Raft_Status.id = id;
#ifdef DEBUG
	printf("RAFT Node assigned id %i\n", Raft_Status.id);
#endif
}

/*******************************/
/**** RAFT TIMER FUNCTIONS  ****/
/*******************************/

		/** If Heartbeat received, reset timer and continue. **/
 void follower_timeout(uint32 timer_id) 
{
#ifdef DEBUG
	OS_printf("\033[31;1mRAFT: Node %i did not receive heartbeat from leader before timeout.\033[0m\n", Raft_Status.id);
#endif

	if (OS_TimerDelete(timer_id) != OS_SUCCESS) {
		followerTimerSet = FALSE;
		OS_printf("RAFT: Could not delete follower timer.\n");
	}

	if (Raft_Status.state != RAFT_CANDIDATE)
		Raft_Status.state = RAFT_CANDIDATE;
}

void set_follower_timer(int32 *status, uint32 *timer_id, uint32 *clock_accuracy_Ptr)
{
	//OS_printf ("\n\n SET FOLLOWER TIMER!\n\n"); //DEBUG

	sleep (1);

	*status = OS_TimerCreate(timer_id, "FOLLOWER_TIMER", clock_accuracy_Ptr, follower_timeout);
	/** If timeout occurs, destroy timer and enter CANDIDATE in follower_timeout()**/
	if (*status!=OS_SUCCESS) 
		OS_printf("RAFT ERROR: Could not create timer in follower function! Ret: %i Node: %i\n", *status, Raft_Status.id);

	sleep(1);

	*status = OS_TimerSet(*timer_id, 1000000, 50000);
	if (*status!=OS_SUCCESS) 
		printf("RAFT ERROR: Could not set timer in follower function! Ret: %i Node: %i\n", *status, Raft_Status.id);
}

void candidate_timeout(uint32 timer_id) 
{
#ifdef DEBUG
	printf("\033[31;1mRaft Candidate Node %i election timed out! Restarting election.\033[0m\n", Raft_Status.id);
#endif
	election_timeout = 1;
}

void set_candidate_timer(int32 *status, uint32 *timer_id, uint32 *clock_accuracy_Ptr)
{
	*status = OS_TimerCreate(timer_id, "CANDIDATE_TIMER", clock_accuracy_Ptr, candidate_timeout);
	if (*status != OS_SUCCESS)
		OS_printf("RAFT ERROR: Could not create timer in candidate function! Ret: %i Node: %i\n", status, Raft_Status.id);

	sleep(1);

	*status = OS_TimerSet(*timer_id, 1000000, *clock_accuracy_Ptr); 	// set timer for half a second
	if (*status != OS_SUCCESS) 
		printf("RAFT ERROR: Could not set timer in candidate function! Ret: %i Node: %i\n", status, Raft_Status.id);
}

/*******************************/
/***** RAFT STATE HANDLERS *****/
/*******************************/

/****************/
/*** FOLLOWER ***/
/****************/

int32 RAFT_follower() 
{
#ifdef DEBUG
  printf("\033[32;1m\nRAFT: Node %i entering FOLLOWER state.\033[0m\n", Raft_Status.id);
#endif 

  int32 status = OS_SUCCESS;
  uint32 timer_id;
  int32 term = -1;
  uint32 clock_accuracy = 100000;

	/** Main follower state loop **/
	int log_index=0;
	while (Raft_Status.state == RAFT_FOLLOWER) 
	{	
		usleep(100000);
		log_index = Raft_Clock(log_index);			

		term = listen_for_heartbeats();
		if (term <= 0) 
		{
			if(!followerTimerSet) {
				//OS_printf("\n\n SET FOLLOWER TIMER, 1st CALL\n\n"); //DEBUG
				set_follower_timer(&status, &timer_id, &clock_accuracy);
				followerTimerSet = TRUE;
			}

		    if (term>Raft_Status.term) 
				{	
				Raft_Status.term = term;

		//		OS_printf("\t\tI FOLLOW: TERM FROM LEADER = %i, I am %i and have term: %i, last log index: %i\n", term, Raft_Status.id, Raft_Status.term, log_index);	//DEBUG
				}
		}
		else
		{
		    if (OS_TimerDelete(timer_id) != OS_SUCCESS)
			{
				followerTimerSet = FALSE;
				OS_printf("RAFT: Could not delete follower timer.\n");
			}
		}
    	/** If RequestVote is received, reset timeout, send response, deal with term **/
		term = listen_for_request_vote();
		if (term != -1) 
		{
		
			//OS_printf ("\n\n SET FOLLOWER TIMER, 2nd CALL\n\n"); 				//DEBUG

			set_follower_timer(&status, &timer_id, &clock_accuracy);
   		}

		//OS_printf ("\n\nIF YOU READ THIS I'M NOT STUCK\n\n"); 				// DEBUG

		//raft_log_update();												//ICAROUS	

	}
	/** End main follower state loop **/
	
  return status;
}

/*****************/
/*** CANDIDATE ***/
/*****************/

int32 RAFT_candidate() 
{
	int32 status = CFE_SUCCESS;
	uint32 timer_id;
	uint32 clock_accuracy = 100000;

	election_timeout = 0;
	int num_votes_received = 1; 				// vote for self
	int32 resp_term = -1;
#ifdef DEBUG
	OS_printf("\033[32;1mRAFT: Node %i entering CANDIDATE state.\033[0m\n", Raft_Status.id);
#endif

	Raft_Status.term++;
	send_request_vote();
	set_candidate_timer(&status, &timer_id, &clock_accuracy);

  	while (Raft_Status.state == RAFT_CANDIDATE && election_timeout == 0) 
	{
    	resp_term = listen_for_vote();
    	if (resp_term == RAFT_DUP_LEADER) 
		{
#ifdef DEBUG
    		OS_printf("\033[31;1mRaft Node %i received message from a leader with a greater term!\033[0m\n", Raft_Status.id);
#endif
      		Raft_Status.state = RAFT_FOLLOWER; 
			break;
    	}
		else if (resp_term > -1) 
			num_votes_received++;

	    if (num_votes_received > (Raft_Status.nodes_in_swarm/2)) 
			Raft_Status.state = RAFT_LEADER; 		// election won

/** If RequestVote received while CANDIDATE, check if THIS NODE should cede to OTHER NODE **/
	int term = -1;
	term = listen_for_request_vote();

 	if (term >= Raft_Status.term ) 
	{
    	send_vote();
		Raft_Status.term = term;
#ifdef DEBUG
   		OS_printf("\033[31;1mRaft Node %i ceding election to CANDIDATE with a greater term!\033[0m\n", Raft_Status.id);
#endif
		Raft_Status.state = RAFT_FOLLOWER;
	}
	if (Raft_Status.state == RAFT_LEADER) {
#ifdef DEBUG
		printf("\033[32;1mRAFT: Node %i won election with %i votes out of %i nodes in swarm.\033[0m\n", Raft_Status.id, num_votes_received, Raft_Status.nodes_in_swarm);
#endif
	}
        }
}

/**************/
/*** LEADER ***/
/**************/

int32 RAFT_leader() 
{
	int term=Raft_Status.term;													// ICAROUS
	int log_index=Raft_Status.index;											// ICAROUS-SBD
	Raft_Status.term++;
#ifdef DEBUG
	printf("\033[32;1mRAFT: Node %i entering LEADER state.\033[0m\n", Raft_Status.id);
#endif

	while (Raft_Status.state == RAFT_LEADER) 
	{
		log_index = Raft_Clock(log_index);										// ICAROUS

	//	OS_printf("\n\t\tI AM LEADER: TERM FROM LEADER = %i, I am %i and have term: %i, NEW LOG INDEX: %i\n", term, Raft_Status.id, term,log_index);					// DEBUG

	// CHECK FOR COMPETING LEADER	
		if ((listen_for_heartbeats()) >= term)
		{
#ifdef DEBUG
			printf("\033[31;1mRaft Node %i received message from a leader with a greater term!\033[0m\n", Raft_Status.id);
#endif
			Raft_Status.term = term;
	    	Raft_Status.state = RAFT_FOLLOWER; 
			return RAFT_FOLLOWER;
		}
		else
		{																		// ICAROUS - debug
    	send_heartbeat();
    	parse_heartbeat_acks(); 
		}

	usleep (200000); 

//	OS_printf("\n\t\t\033[34;1mBEGIN NEW LEADER CYCLE. \033[1m""\033[52;1mRAFT LOG INDEX = %i\033[1m", log_index);			// RAFT  DEBUG
	}
#ifdef DEBUG
	OS_printf("\033[31;1m\n\n\tRaft Node %i JUMPED OUT OF INDEX LOOP! - LEADERSHIP STATE INTERRUPTED\033[0m\n", Raft_Status.id);	// RAFT  DEBUG
#endif
}

/*======================================================================================
** End of file raft_app.c
**=====================================================================================*/
