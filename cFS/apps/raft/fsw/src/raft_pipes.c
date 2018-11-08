
/*=======================================================================================
** File Name:  raft_pipes.c
**
** Title:  Pipe Initializations and Subscriptions for RAFT Application
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

/*=====================================================================================
** Name: RAFT_InitPipe
** Purpose: To initialize all message pipes and subscribe to messages for RAFT application
** Returns:
**    int32 iStatus - Status of initialization
** Routines Called:
**    CFE_SB_CreatePipe
**    CFE_SB_Subscribe
**    CFE_ES_WriteToSysLog
** Called By:
**    RAFT_InitApp
** Global Outputs/Writes:
**    g_RAFT_AppData.usSchPipeDepth
**    g_RAFT_AppData.cSchPipeName
**    g_RAFT_AppData.SchPipeId
**    g_RAFT_AppData.usCmdPipeDepth
**    g_RAFT_AppData.cCmdPipeName
**    g_RAFT_AppData.CmdPipeId
**    g_RAFT_AppData.usTlmPipeDepth
**    g_RAFT_AppData.cTlmPipeName
**    g_RAFT_AppData.TlmPipeId
**=====================================================================================*/

int32 RAFT_InitPipe()
{
    int32  iStatus=CFE_SUCCESS;

	/*****************************************/
    /* Init schedule pipe */
	/*****************************************/
/*
    g_RAFT_AppData.usSchPipeDepth = RAFT_SCH_PIPE_DEPTH;
    memset((void*)g_RAFT_AppData.cSchPipeName, '\0', sizeof(g_RAFT_AppData.cSchPipeName));
    strncpy(g_RAFT_AppData.cSchPipeName, "RAFT_SCH_PIPE", OS_MAX_API_NAME-1);
*/
	/*****************************************/
    /* Subscribe to Wakeup messages */
	/*****************************************/
/*
    iStatus = CFE_SB_CreatePipe(&g_RAFT_AppData.SchPipeId,
                                 g_RAFT_AppData.usSchPipeDepth,
                                 g_RAFT_AppData.cSchPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        iStatus = CFE_SB_SubscribeEx(RAFT_WAKEUP_MID, g_RAFT_AppData.SchPipeId, CFE_SB_Default_Qos, 1);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("RAFT - Sch Pipe failed to subscribe to RAFT_WAKEUP_MID. (0x%08X)\n", iStatus);
            goto RAFT_InitPipe_Exit_Tag;
        }

    }
    else
    {
        CFE_ES_WriteToSysLog("RAFT - Failed to create SCH pipe (0x%08X)\n", iStatus);
        goto RAFT_InitPipe_Exit_Tag;
    }
*/
	/*****************************************/
    /* INIT COMMAND PIPE DATA */
	/*****************************************/

    g_RAFT_AppData.usCmdPipeDepth = RAFT_CMD_PIPE_DEPTH ;
    memset((void*)g_RAFT_AppData.cCmdPipeName, '\0', sizeof(g_RAFT_AppData.cCmdPipeName));
    strncpy(g_RAFT_AppData.cCmdPipeName, "RAFT_CMD_PIPE", OS_MAX_API_NAME-1);

	/*****************************************/
    /* CREATION for COMMAND MESSAGE PIPE */
	/*****************************************/

    iStatus = CFE_SB_CreatePipe(&g_RAFT_AppData.CmdPipeId,
                                 g_RAFT_AppData.usCmdPipeDepth,
                                 g_RAFT_AppData.cCmdPipeName);
    if (iStatus == CFE_SUCCESS)
    {
		/*****************************************/
        /* SUBSCRIBE to COMMAND MESSAGE TOPIC */
		/*****************************************/
 
       iStatus = CFE_SB_Subscribe(RAFT_CMD_MID, g_RAFT_AppData.CmdPipeId);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("RAFT - CMD Pipe failed to subscribe to RAFT_CMD_MID. (0x%08X)\n", iStatus);
            goto RAFT_InitPipe_Exit_Tag;
        }

        iStatus = CFE_SB_Subscribe(RAFT_SEND_HK_MID, g_RAFT_AppData.CmdPipeId);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("RAFT - CMD Pipe failed to subscribe to RAFT_SEND_HK_MID. (0x%08X)\n", iStatus);
            goto RAFT_InitPipe_Exit_Tag;
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("RAFT - Failed to create CMD pipe (0x%08X)\n", iStatus);
        goto RAFT_InitPipe_Exit_Tag;
    }

	/*****************************************/
    /* INIT DATA and CREATE TELEMETRY PIPE */
	/*****************************************/

    g_RAFT_AppData.usTlmPipeDepth = RAFT_TLM_PIPE_DEPTH;
    memset((void*)g_RAFT_AppData.cTlmPipeName, '\0', sizeof(g_RAFT_AppData.cTlmPipeName));
    strncpy(g_RAFT_AppData.cTlmPipeName, "RAFT_TLM_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to telemetry messages on the telemetry pipe */
    iStatus = CFE_SB_CreatePipe(&g_RAFT_AppData.TlmPipeId,
                                 g_RAFT_AppData.usTlmPipeDepth,
                                 g_RAFT_AppData.cTlmPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        /* TODO:  Add CFE_SB_Subscribe() calls for other apps' output data here.
        **
        ** Examples:
        **     CFE_SB_Subscribe(GNCEXEC_OUT_DATA_MID, g_RAFT_AppData.TlmPipeId);
        */
    }
    else
    {
        CFE_ES_WriteToSysLog("RAFT - Failed to create TLM pipe (0x%08X)\n", iStatus);
        goto RAFT_InitPipe_Exit_Tag;
    }


	/*****************************************/
    /** INIT DATA and CREATE HEARTBEAT PIPE **/
	/*****************************************/

    g_RAFT_AppData.HBPipeDepth = RAFT_TLM_PIPE_DEPTH;
    memset((void*)g_RAFT_AppData.cTlmPipeName, '\0', sizeof(g_RAFT_AppData.cTlmPipeName));
    strncpy(g_RAFT_AppData.HBPipeName, "RAFT_HB_PIPE", OS_MAX_API_NAME-1);

    iStatus = CFE_SB_CreatePipe(&g_RAFT_AppData.HBPipeId,
                                           g_RAFT_AppData.HBPipeDepth,
                                           g_RAFT_AppData.HBPipeName);
    if (iStatus != CFE_SUCCESS) 
		printf("RAFT Node %i: HeartBeat Pipe Creation FAILED with status %i. Pipe Id: %i\n", Raft_Status.id, iStatus, g_RAFT_AppData.HBPipeId);
	else
	{
		/*****************************************/
    	/* SUBSCRIBE to HEARTBEAT MESSAGES */
		/*****************************************/
	    iStatus = CFE_SB_Subscribe(RAFT_HB_ID, g_RAFT_AppData.HBPipeId);
	    if (iStatus != CFE_SUCCESS) 
			printf("RAFT Node %i: Heartbeat Subscription FAILED with status %i\n", Raft_Status.id, iStatus);
		else    		
		printf("RAFT Node %i subscribed to heartbeats.\n", Raft_Status.id);
	}
	/*****************************************/
    /** INIT DATA and CREATE HB ACK PIPE **/
	/*****************************************/

    g_RAFT_AppData.HBACKPipeDepth = RAFT_TLM_PIPE_DEPTH;
   	strncpy(g_RAFT_AppData.HBACKPipeName, "RAFT_HBACK_PIPE", OS_MAX_API_NAME-1);

    iStatus = CFE_SB_CreatePipe(&g_RAFT_AppData.HBACKPipeId,
                                           g_RAFT_AppData.HBACKPipeDepth,
                                           g_RAFT_AppData.HBACKPipeName);
    if (iStatus != CFE_SUCCESS)
		printf("RAFT Node %i: HeartBeat ACK Pipe Creation FAILED with status %i. Pipe Id: %i\n",
                                           Raft_Status.id, iStatus, g_RAFT_AppData.HBACKPipeId);
	else
	{
		/*****************************************/
		/* SUBSCRIBE to HEART ACK MESSAGE */
		/*****************************************/
		iStatus = CFE_SB_Subscribe(RAFT_HBACK_ID, g_RAFT_AppData.HBACKPipeId);
    	if (iStatus != CFE_SUCCESS)
			printf("RAFT Node %i: Heartbeat ACK Subscription FAILED with status %i\n", Raft_Status.id, iStatus);
		else
	    	printf("RAFT Node %i subscribed to heartbeat acks.\n", Raft_Status.id);
	}

	/*****************************************/
    /* INIT DATA and CREATE RequestVote PIPE */
	/*****************************************/

    g_RAFT_AppData.RVPipeDepth = RAFT_TLM_PIPE_DEPTH;
    memset((void*)g_RAFT_AppData.RVPipeName, '\0', sizeof(g_RAFT_AppData.RVPipeName));
    strncpy(g_RAFT_AppData.RVPipeName, "RAFT_RV_PIPE", OS_MAX_API_NAME-1);

    iStatus = CFE_SB_CreatePipe(&g_RAFT_AppData.RVPipeId,
                                           g_RAFT_AppData.RVPipeDepth,
                                           g_RAFT_AppData.RVPipeName);
    if (iStatus != CFE_SUCCESS) 
		printf("RAFT Node %i: RequestVote Pipe Creation FAILED with status %i. Pipe Id: %i\n",
                    Raft_Status.id, iStatus, g_RAFT_AppData.RVPipeId);
	else
	{
		/*****************************************/
    	/** Subscribe to RequestVote messages **/
		/*****************************************/
	    iStatus = CFE_SB_Subscribe(RAFT_RV_ID, g_RAFT_AppData.RVPipeId);
	    if (iStatus != CFE_SUCCESS) 
			printf("RAFT Node %i: RequestVote Subscription FAILED with status %i\n",
                    Raft_Status.id, iStatus);
		else
	    	printf("RAFT Node %i subscribed to Vote Requests.\n", Raft_Status.id);
	}

	/*****************************************/
    /* INIT DATA & CREATE VoteResponse PIPE */
	/*****************************************/

    g_RAFT_AppData.VotePipeDepth = RAFT_TLM_PIPE_DEPTH;
    memset((void*)g_RAFT_AppData.VotePipeName, '\0', sizeof(g_RAFT_AppData.VotePipeName));
    strncpy(g_RAFT_AppData.VotePipeName, "RAFT_VOTE_PIPE", OS_MAX_API_NAME-1);
  
	iStatus = CFE_SB_CreatePipe(&g_RAFT_AppData.VotePipeId,
                                           g_RAFT_AppData.VotePipeDepth,
                                           g_RAFT_AppData.VotePipeName);
    if (iStatus != CFE_SUCCESS) 
		printf("RAFT Node %i: Vote Pipe Creation FAILED with status %i. Pipe Id: %i\n",
                    Raft_Status.id, iStatus, g_RAFT_AppData.VotePipeId);
	else
	{
		/*****************************************/
		/** Subscribe to VoteResponse messages **/
		/*****************************************/

		iStatus = CFE_SB_Subscribe(RAFT_VOTE_ID, g_RAFT_AppData.VotePipeId);
		if (iStatus != CFE_SUCCESS) 
			printf("RAFT Node %i: Vote Subscription FAILED with status %i\n",
                   Raft_Status.id, iStatus);
		else
	    	printf("RAFT Node %i subscribed to Ballots.\n", Raft_Status.id);
	}
		
	/*****************************************/
	/** RAFT PIPE: CREATE/SUBSCRIBE SECTION **/
	/*****************************************/

	/*****************************************/
	/** RAFT PIPE: CREATE/SUBSCRIBE SECTION **/
	/*****************************************/



	/***************************************/
    /* Init for: MERGER TELEMETRY MESSAGES */					//MERGER
	/***************************************/

    g_RAFT_AppData.MERGERPipeDepth = RAFT_TLM_PIPE_DEPTH;
    memset((void*)g_RAFT_AppData.MERGERPipeName, '\0', sizeof(g_RAFT_AppData.MERGERPipeName));
    strncpy(g_RAFT_AppData.MERGERPipeName, "RAFT_MERGER_PIPE", OS_MAX_API_NAME-1);

    iStatus = CFE_SB_CreatePipe(&g_RAFT_AppData.MERGERPipeId,
                                           g_RAFT_AppData.MERGERPipeDepth,
                                           g_RAFT_AppData.MERGERPipeName);
	 if (iStatus != CFE_SUCCESS) 
			printf("RAFT Node %i: MERGER Pipe Creation FAILED with status %i. Pipe Id: %i\n", Raft_Status.id, iStatus, g_RAFT_AppData.MERGERPipeId);
	else
	{
		/***************************************/
    	/* Subscribe to MERGER TELEME MESSAGES */					//MERGER
		/***************************************/

	    iStatus = CFE_SB_Subscribe(MERGER_RTD_MID, g_RAFT_AppData.MERGERPipeId);
	    if (iStatus != CFE_SUCCESS) 
			printf("RAFT Node %i: MERGER Subscription FAILED with status %i\n", Raft_Status.id, iStatus);
	    else
			printf("RAFT Node %i subscribed to MERGER pushes.\n", Raft_Status.id);
	}

	/*****************************/
    /** Init Raft Log Push Pipe **/									//ICAROUS
	/*****************************/

    g_RAFT_AppData.LOGPUSHPipeDepth = RAFT_TLM_PIPE_DEPTH;
   	strncpy(g_RAFT_AppData.LOGPUSHPipeName, "RAFT_LOGPUSH_PIPE", OS_MAX_API_NAME-1);
   	iStatus = CFE_SB_CreatePipe(&g_RAFT_AppData.LOGPUSHPipeId,
                                           g_RAFT_AppData.LOGPUSHPipeDepth,
                                           g_RAFT_AppData.LOGPUSHPipeName);
    if (iStatus != CFE_SUCCESS) 
		printf("RAFT Node %i: Log Push Pipe Creation FAILED with status %i. Pipe Id: %i\n", Raft_Status.id, iStatus, g_RAFT_AppData.LOGPUSHPipeId);
	else
	{
		/***************************************/
    	/* Subscribe to Raft LOG PUSH MESSAGES */								//ICAROUS
		/***************************************/
    iStatus = CFE_SB_Subscribe(RAFT_LOGPUSH_ID, g_RAFT_AppData.LOGPUSHPipeId);
  	
	if (iStatus != CFE_SUCCESS) 
		printf("RAFT Node %i: LOG PUSH Subscription FAILED with status %i\n", Raft_Status.id, iStatus);
    else 
		printf("RAFT Node %i subscribed to log pushes.\n", Raft_Status.id);
	}

	/*******************************/
    /*Init Raft Log Accumulate Pipe*/											//RAFT
	/*******************************/

    g_RAFT_AppData.LOGACCPipeDepth = RAFT_TLM_PIPE_DEPTH;
    strncpy(g_RAFT_AppData.LOGACCPipeName, "RAFT_LOGACC_PIPE", OS_MAX_API_NAME-1);
    
	iStatus = CFE_SB_CreatePipe(&g_RAFT_AppData.LOGACCPipeId,
                                           g_RAFT_AppData.LOGACCPipeDepth,
                                           g_RAFT_AppData.LOGACCPipeName);
	if (iStatus != CFE_SUCCESS) 
			printf("RAFT Node %i: Log Accumulate Pipe Creation FAILED with status %i. Pipe Id: %i\n", Raft_Status.id, iStatus, g_RAFT_AppData.LOGACCPipeId);
	else 
	{
		/*****************************************/
	    /* Subscribe to Raft LOG ACC: ENTRY MSGs */								//RAFT
		/*****************************************/


	    iStatus = CFE_SB_Subscribe(RAFT_LOGACC_ID, g_RAFT_AppData.LOGACCPipeId);

	    if (iStatus != CFE_SUCCESS) 
			printf("RAFT Node %i: LOG ENTRY Pipe Subscription FAILED with status %i\n", Raft_Status.id, iStatus);
		else
		    printf("RAFT Node %i subscribed to LOG ENTRY Pipe.\n", Raft_Status.id);
	}




	/*******************************/
    /** Init Raft Log Update Pipe **/											//RAFT
	/*******************************/

	/*

    g_RAFT_AppData.LOGUDPipeDepth = RAFT_TLM_PIPE_DEPTH;
    strncpy(g_RAFT_AppData.LOGUDPipeName, "RAFT_LOGUD_PIPE", OS_MAX_API_NAME-1);
    
	iStatus = CFE_SB_CreatePipe(&g_RAFT_AppData.LOGUDPipeId,
                                           g_RAFT_AppData.LOGUDPipeDepth,
                                           g_RAFT_AppData.LOGUDPipeName);
	if (iStatus != CFE_SUCCESS) 
			printf("RAFT Node %i: Log Update Pipe Creation FAILED with status %i. Pipe Id: %i\n", Raft_Status.id, iStatus, g_RAFT_AppData.LOGUDPipeId);
	else 
	{
		/*****************************************/
	    /* Subscribe to Raft LOG UPDATE MESSAGES */						//ICAROUS
		/*****************************************/
	/*

	    iStatus = CFE_SB_Subscribe(RAFT_LOGUD_ID, g_RAFT_AppData.LOGUDPipeId);

	    if (iStatus != CFE_SUCCESS) 
			printf("RAFT Node %i: Log Update Pipe Subscription FAILED with status %i\n", Raft_Status.id, iStatus);
		else
		    printf("RAFT Node %i subscribed to Log Update Pipe.\n", Raft_Status.id);
	}
	*/

RAFT_InitPipe_Exit_Tag:
    return (iStatus);
}

