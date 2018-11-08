/*=======================================================================================
** File Name:  raft_processing.c
**
** Title:  I/O Data Processing Initialization and Functions for RAFT Application
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
** Name: RAFT_InitData
** Purpose: To initialize global variables used by RAFT application
** Returns:
**    int32 iStatus - Status of initialization
** Routines Called:
**    CFE_SB_InitMsg
** Called By:
**    RAFT_InitApp
** Global Outputs/Writes:
**    g_RAFT_AppData.InData
**    g_RAFT_AppData.OutData
**    g_RAFT_AppData.HkTlm
**=====================================================================================*/

int32 RAFT_InitData()
{
    int32  iStatus=CFE_SUCCESS;

    /* Init input data */
    memset((void*)&g_RAFT_AppData.InData, 0x00, sizeof(g_RAFT_AppData.InData));

    /* Init output data */
    memset((void*)&g_RAFT_AppData.OutData, 0x00, sizeof(g_RAFT_AppData.OutData));
    CFE_SB_InitMsg(&g_RAFT_AppData.OutData,
                   RAFT_OUT_DATA_MID, sizeof(g_RAFT_AppData.OutData), TRUE);

    /* Init housekeeping packet */
    memset((void*)&g_RAFT_AppData.HkTlm, 0x00, sizeof(g_RAFT_AppData.HkTlm));
    CFE_SB_InitMsg(&g_RAFT_AppData.HkTlm,
                   RAFT_HK_TLM_MID, sizeof(g_RAFT_AppData.HkTlm), TRUE);

    return (iStatus);
}

/*=====================================================================================
** Name: RAFT_ProcessNewData
** Purpose: To process incoming data subscribed by RAFT application
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
** Called By:
**    RAFT_RcvMsg
============================================*/

void RAFT_ProcessNewData()
{
    int iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   TlmMsgPtr=NULL;
    CFE_SB_MsgId_t  TlmMsgId;

    /* Process telemetry messages till the pipe is empty */
    while (CFE_ES_RunLoop(&g_RAFT_AppData.uiRunStatus) == TRUE)
    {
        iStatus = CFE_SB_RcvMsg(&TlmMsgPtr, g_RAFT_AppData.TlmPipeId, CFE_SB_POLL);
        if (iStatus == CFE_SUCCESS)
        {
            TlmMsgId = CFE_SB_GetMsgId(TlmMsgPtr);
            switch (TlmMsgId)
            {
                /* TODO:  Add code to process all subscribed data here
                **
                ** Example:
                **     case NAV_OUT_DATA_MID:
                **         RAFT_ProcessNavData(TlmMsgPtr);
                **         break;
                */

                default:
                    CFE_EVS_SendEvent(RAFT_MSGID_ERR_EID, CFE_EVS_ERROR,
                                      "RAFT - Recvd invalid TLM msgId (0x%08X)", TlmMsgId);
                    break;
            }
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else
        {
            CFE_EVS_SendEvent(RAFT_PIPE_ERR_EID, CFE_EVS_ERROR,
                  "RAFT: CMD pipe read error (0x%08X)", iStatus);
            g_RAFT_AppData.uiRunStatus = CFE_ES_APP_ERROR;
            break;
        }
    }
}


/*=====================================================================================
** Name: RAFT_ProcessNewCmds
** Purpose: To process incoming command messages for RAFT application
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    RAFT_ProcessNewAppCmds
**    RAFT_ReportHousekeeping
**=====================================================================================*/

void RAFT_ProcessNewCmds()
{
    int iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   CmdMsgPtr=NULL;
    CFE_SB_MsgId_t  CmdMsgId;

    /* Process command messages till the pipe is empty */
    while (CFE_ES_RunLoop(&g_RAFT_AppData.uiRunStatus) == TRUE)
    {
        iStatus = CFE_SB_RcvMsg(&CmdMsgPtr, g_RAFT_AppData.CmdPipeId, CFE_SB_POLL);
        if(iStatus == CFE_SUCCESS)
        {
            CmdMsgId = CFE_SB_GetMsgId(CmdMsgPtr);
            switch (CmdMsgId)
            {
                case RAFT_CMD_MID:
                    RAFT_ProcessNewAppCmds(CmdMsgPtr);
                    break;

                case RAFT_SEND_HK_MID:
                    RAFT_ReportHousekeeping();
                    break;

                /* TODO:  Add code to process other subscribed commands here
                **
                ** Example:
                **     case CFE_TIME_DATA_CMD_MID:
                **         RAFT_ProcessTimeDataCmd(CmdMsgPtr);
                **         break;
                */

                default:
                    CFE_EVS_SendEvent(RAFT_MSGID_ERR_EID, CFE_EVS_ERROR, "RAFT - Recvd invalid CMD msgId (0x%08X)", CmdMsgId);
                    break;
            }
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else
        {
            CFE_EVS_SendEvent(RAFT_PIPE_ERR_EID, CFE_EVS_ERROR, "RAFT: CMD pipe read error (0x%08X)", iStatus);
            g_RAFT_AppData.uiRunStatus = CFE_ES_APP_ERROR;
            break;
        }
    }
}

/*=====================================================================================
** Name: RAFT_ProcessNewAppCmds
** Purpose: To process command messages targeting RAFT application
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr - new command message pointer
** Routines Called:
**    CFE_SB_GetCmdCode
**    CFE_EVS_SendEvent
** Called By:
**    RAFT_ProcessNewCmds
**=====================================================================================*/

void RAFT_ProcessNewAppCmds(CFE_SB_Msg_t* MsgPtr)
{
    uint32  uiCmdCode=0;

    if (MsgPtr != NULL)
    {
        uiCmdCode = CFE_SB_GetCmdCode(MsgPtr);
        switch (uiCmdCode)
        {
            case RAFT_NOOP_CC:
                g_RAFT_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(RAFT_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "RAFT - Recvd NOOP cmd (%d)", uiCmdCode);
                break;

            case RAFT_RESET_CC:
                g_RAFT_AppData.HkTlm.usCmdCnt = 0;
                g_RAFT_AppData.HkTlm.usCmdErrCnt = 0;
                CFE_EVS_SendEvent(RAFT_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "RAFT - Recvd RESET cmd (%d)", uiCmdCode);
                break;

            case RAFT_FOLLOWER_CC:
                g_RAFT_AppData.HkTlm.usCmdCnt = 0;
                g_RAFT_AppData.HkTlm.usCmdErrCnt = 0;
				Raft_Status.state = RAFT_FOLLOWER;
printf ("\033[31;1m\t\tRAFT NODE COMMANDED TO FOLLOWER MODE.\033[0m\n");
                CFE_EVS_SendEvent(RAFT_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "RAFT - FOLLOWER SELECT CMD (%d)", uiCmdCode);
                break;

            case RAFT_CANDIDATE_CC:
                g_RAFT_AppData.HkTlm.usCmdCnt = 0;
                g_RAFT_AppData.HkTlm.usCmdErrCnt = 0;
				Raft_Status.state = RAFT_CANDIDATE;
printf ("\033[31;1m\t\tRAFT NODE COMMANDED TO CANDIDATE MODE.\033[0m\n");
                CFE_EVS_SendEvent(RAFT_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "RAFT - FOLLOWER SELECT CMD (%d)", uiCmdCode);
                break;

            case RAFT_LEADER_CC:
                g_RAFT_AppData.HkTlm.usCmdCnt = 0;
                g_RAFT_AppData.HkTlm.usCmdErrCnt = 0;
				Raft_Status.state = RAFT_LEADER;
printf ("\033[31;1m\t\tRAFT NODE COMMANDED TO LEADER MODE.\033[0m\n");
                CFE_EVS_SendEvent(RAFT_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "RAFT - FOLLOWER SELECT CMD (%d)", uiCmdCode);
                break;
            /* TODO:  Add code to process the rest of the RAFT commands here */

            default:
                g_RAFT_AppData.HkTlm.usCmdErrCnt++;
                CFE_EVS_SendEvent(RAFT_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "RAFT - Recvd invalid cmdId (%d)", uiCmdCode);
                break;
        }
    }
}

/*=====================================================================================
** Name: RAFT_ReportHousekeeping
** Purpose: To send housekeeping message
**=====================================================================================*/

void RAFT_ReportHousekeeping()
{
    /* TODO:  Add code to update housekeeping data, if needed, here.  */

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_RAFT_AppData.HkTlm);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&g_RAFT_AppData.HkTlm);
}

/*=====================================================================================
** Name: RAFT_SendOutData
** Purpose: To publish 1-Wakeup cycle output data
** Called By:
**    RAFT_RcvMsg
**=====================================================================================*/

void RAFT_SendOutData()
{
    /* TODO:  Add code to update output data, if needed, here.  */

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_RAFT_AppData.OutData);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&g_RAFT_AppData.OutData);
}


/*=====================================================================================
** Name: RAFT_VerifyCmdLength
** Purpose: To verify command length for a particular command message
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr      - command message pointer
**    uint16         usExpLength - expected command length
** Returns:
**    boolean bResult - result of verification
** Called By:
**    RAFT_ProcessNewCmds
**=====================================================================================*/

boolean RAFT_VerifyCmdLength(CFE_SB_Msg_t* MsgPtr,
                           uint16 usExpectedLen)
{
    boolean bResult=FALSE;
    uint16  usMsgLen=0;

    if (MsgPtr != NULL)
    {
        usMsgLen = CFE_SB_GetTotalMsgLength(MsgPtr);

        if (usExpectedLen != usMsgLen)
        {
            CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(MsgPtr);
            uint16 usCmdCode = CFE_SB_GetCmdCode(MsgPtr);

            CFE_EVS_SendEvent(RAFT_MSGLEN_ERR_EID, CFE_EVS_ERROR,
                              "RAFT - Rcvd invalid msgLen: msgId=0x%08X, cmdCode=%d, "
                              "msgLen=%d, expectedLen=%d",
                              MsgId, usCmdCode, usMsgLen, usExpectedLen);
            g_RAFT_AppData.HkTlm.usCmdErrCnt++;
        }
    }

    return (bResult);
}

