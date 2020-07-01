/******************************************************************************
 ** \file sbn_app.c
 **
 **      Copyright (c) 2004-2006, United States government as represented by the
 **      administrator of the National Aeronautics Space Administration.
 **      All rights reserved. This software(cFE) was created at NASA's Goddard
 **      Space Flight Center pursuant to government contracts.
 **
 **      This software may be used only pursuant to a United States government
 **      sponsored project and the United States government may not be charged
 **      for use thereof.
 **
 ** Purpose:
 **      This file contains source code for the Software Bus Network
 **      Application.
 **
 ** Authors:   J. Wilmot/GSFC Code582
 **            R. McGraw/SSI
 **            C. Knight/ARC Code TI
 ******************************************************************************/

/*
 ** Include Files
 */
#include <fcntl.h>

#include "sbn_pack.h"
#include "sbn_app.h"
#include "cfe_sb_events.h" /* For event message IDs */
#include "cfe_es.h" /* PerfLog */
#include "cfe_platform_cfg.h"
#include "cfe_msgids.h"
#include "cfe_version.h"

/** \brief SBN global application data, indexed by AppID. */
SBN_App_t SBN;

#include <string.h>
#include "sbn_app.h"

static SBN_Status_t UnloadModules(void)
{
    SBN_ModuleIdx_t i = 0;

    for(i = 0; i < SBN_MAX_MOD_CNT; i++)
    for(i = 0; SBN.ProtocolModules[i] != 0 && i < SBN_MAX_MOD_CNT; i++)
    {
        if(OS_ModuleUnload(SBN.ProtocolModules[i]) != OS_SUCCESS)
        {
            OS_printf("Unable to unload module ID %d for Protocol ID %d\n",
                SBN.ProtocolModules[i], i);
        }/* end if */
    }/* end for */

    for(i = 0; SBN.FilterModules[i] != 0 && i < SBN_MAX_MOD_CNT; i++)
    {
        if(OS_ModuleUnload(SBN.FilterModules[i]) != OS_SUCCESS)
        {
            OS_printf("Unable to unload module ID %d for Filter ID %d\n",
                SBN.FilterModules[i], i);
        }/* end if */
    }/* end for */

    return SBN_SUCCESS;
}/* end UnloadModules() */

/**
 * \brief Packs a CCSDS message with an SBN message header.
 * \note Ensures the SBN fields (CPU ID, MsgSz) and CCSDS message headers
 *       are in network (big-endian) byte order.
 * \param SBNBuf[out] The buffer to pack into.
 * \param MsgType[in] The SBN message type.
 * \param MsgSz[in] The size of the payload.
 * \param ProcessorID[in] The ProcessorID of the sender (should be CFE_CPU_ID)
 * \param Msg[in] The payload (CCSDS message or SBN sub/unsub.)
 */
void SBN_PackMsg(void *SBNBuf, SBN_MsgSz_t MsgSz, SBN_MsgType_t MsgType,
    CFE_ProcessorID_t ProcessorID, void *Msg)
{
    Pack_t Pack;
    Pack_Init(&Pack, SBNBuf, MsgSz + SBN_PACKED_HDR_SZ, 0);

    Pack_UInt16(&Pack, MsgSz);
    Pack_UInt8(&Pack, MsgType);
    Pack_UInt32(&Pack, ProcessorID);

    if(!Msg || !MsgSz)
    {
        /* valid to have a NULL pointer/empty size payload */
        return;
    }/* end if */

    Pack_Data(&Pack, Msg, MsgSz);
}/* end SBN_PackMsg */

/**
 * \brief Unpacks a CCSDS message with an SBN message header.
 * \param SBNBuf[in] The buffer to unpack.
 * \param MsgTypePtr[out] The SBN message type.
 * \param MsgSzPtr[out] The payload size.
 * \param ProcessorID[out] The ProcessorID of the sender.
 * \param Msg[out] The payload (a CCSDS message, or SBN sub/unsub).
 * \return TRUE if we were able to unpack the message.
 *
 * \note Ensures the SBN fields (CPU ID, MsgSz) and CCSDS message headers
 *       are in platform byte order.
 * \todo Use a type for SBNBuf.
 */
bool SBN_UnpackMsg(void *SBNBuf, SBN_MsgSz_t *MsgSzPtr, SBN_MsgType_t *MsgTypePtr,
    CFE_ProcessorID_t *ProcessorIDPtr, void *Msg)
{
    uint8 t = 0;
    Unpack_t Unpack; Unpack_Init(&Unpack, SBNBuf, SBN_MAX_PACKED_MSG_SZ);
    Unpack_UInt16(&Unpack, MsgSzPtr);
    Unpack_UInt8(&Unpack, &t);
    *MsgTypePtr = t;
    Unpack_UInt32(&Unpack, ProcessorIDPtr);

    if(!*MsgSzPtr)
    {
        return TRUE;
    }/* end if */

    if(*MsgSzPtr > CFE_SB_MAX_SB_MSG_SIZE)
    {
        return FALSE;
    }/* end if */

    Unpack_Data(&Unpack, Msg, *MsgSzPtr);

    return TRUE;
}/* end SBN_UnpackMsg */

/* Use a struct for all local variables in the task so we can specify exactly
 * how large of a stack we need for the task.
 */

typedef struct
{
    SBN_Status_t Status;
    OS_TaskID_t RecvTaskID;
    SBN_PeerIdx_t PeerIdx;
    SBN_NetIdx_t NetIdx;
    SBN_PeerInterface_t *Peer;
    SBN_NetInterface_t *Net;
    CFE_ProcessorID_t ProcessorID;
    SBN_MsgType_t MsgType;
    SBN_MsgSz_t MsgSz;
    uint8 Msg[CFE_SB_MAX_SB_MSG_SIZE];
} RecvPeerTaskData_t;

static void RecvPeerTask(void)
{
    RecvPeerTaskData_t D;
    memset(&D, 0, sizeof(D));
    if(CFE_ES_RegisterChildTask() != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_PEERTASK_EID, CFE_EVS_ERROR,
            "unable to register child task");
        return;
    }/* end if */

    D.RecvTaskID = OS_TaskGetId();

    for(D.NetIdx = 0; D.NetIdx < SBN.NetCnt; D.NetIdx++)
    {
        D.Net = &SBN.Nets[D.NetIdx];
        if(!D.Net->Configured)
        {
            continue;
        }

        for(D.PeerIdx = 0; D.PeerIdx < D.Net->PeerCnt; D.PeerIdx++)
        {
            D.Peer = &D.Net->Peers[D.PeerIdx];
            if(D.Peer->RecvTaskID == D.RecvTaskID)
            {
                break;
            }/* end if */
        }/* end for */

        if(D.PeerIdx < D.Net->PeerCnt)
        {
            break;
        }/* end if */
    }/* end for */

    if(D.NetIdx == SBN.NetCnt)
    {
        CFE_EVS_SendEvent(SBN_PEERTASK_EID, CFE_EVS_ERROR,
            "unable to connect task to peer struct");
        return;
    }/* end if */

    while(1)
    {
        SBN_Status_t Status;
        Status = D.Net->IfOps->RecvFromPeer(D.Net, D.Peer,
            &D.MsgType, &D.MsgSz, &D.ProcessorID, &D.Msg);

        if(Status == SBN_IF_EMPTY)
        {
            continue; /* no (more) messages */
        }/* end if */

        if(Status == SBN_SUCCESS)
        {
            OS_GetLocalTime(&D.Peer->LastRecv);

            SBN_ProcessNetMsg(D.Net, D.MsgType, D.ProcessorID, D.MsgSz, &D.Msg);
        }
        else
        {
            CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
                "recv error (%d)", Status);
            D.Peer->RecvErrCnt++;
        }/* end if */
    }/* end while */
}/* end RecvPeerTask */

typedef struct
{
    SBN_NetIdx_t NetIdx;
    SBN_NetInterface_t *Net;
    SBN_PeerInterface_t *Peer;
    SBN_Status_t Status;
    OS_TaskID_t RecvTaskID;
    CFE_ProcessorID_t ProcessorID;
    SBN_MsgType_t MsgType;
    SBN_MsgSz_t MsgSz;
    uint8 Msg[CFE_SB_MAX_SB_MSG_SIZE];
} RecvNetTaskData_t;

static void RecvNetTask(void)
{
    RecvNetTaskData_t D;
    memset(&D, 0, sizeof(D));
    if(CFE_ES_RegisterChildTask() != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_PEERTASK_EID, CFE_EVS_ERROR, "unable to register child task");
        return;
    }/* end if */

    D.RecvTaskID = OS_TaskGetId();

    for(D.NetIdx = 0; D.NetIdx < SBN.NetCnt; D.NetIdx++)
    {
        D.Net = &SBN.Nets[D.NetIdx];
        if(D.Net->RecvTaskID == D.RecvTaskID)
        {
            break;
        }/* end if */
    }/* end for */

    if(D.NetIdx == SBN.NetCnt)
    {
        CFE_EVS_SendEvent(SBN_PEERTASK_EID, CFE_EVS_ERROR,
            "unable to connect task to net struct");
        return;
    }/* end if */

    while(1)
    {
        SBN_Status_t Status = SBN_SUCCESS;

        Status = D.Net->IfOps->RecvFromNet(D.Net, &D.MsgType,
            &D.MsgSz, &D.ProcessorID, &D.Msg);

        if(Status == SBN_IF_EMPTY)
        {
            continue; /* no (more) messages */
        }/* end if */

        if(Status != SBN_SUCCESS)
        {
            return;
        }/* end if */

        D.Peer = SBN_GetPeer(D.Net, D.ProcessorID);
        if(!D.Peer)
        {
            CFE_EVS_SendEvent(SBN_PEERTASK_EID, CFE_EVS_ERROR,
                "unknown peer (ProcessorID=%d)", D.ProcessorID);
            return;
        }/* end if */

        OS_GetLocalTime(&D.Peer->LastRecv);

        SBN_ProcessNetMsg(D.Net, D.MsgType, D.ProcessorID, D.MsgSz, &D.Msg);
    }/* end while */
}/* end RecvNetTask */

/**
 * Checks all interfaces for messages from peers.
 * Receive messages from the specified peer, injecting them onto the local
 * software bus.
 */
void SBN_RecvNetMsgs(void)
{
    SBN_Status_t Status = 0;
    uint8 Msg[CFE_SB_MAX_SB_MSG_SIZE];

    SBN_NetIdx_t NetIdx = 0;
    for(NetIdx = 0; NetIdx < SBN.NetCnt; NetIdx++)
    {
        SBN_NetInterface_t *Net = &SBN.Nets[NetIdx];
        SBN_MsgType_t MsgType;
        SBN_MsgSz_t MsgSz;
        CFE_ProcessorID_t ProcessorID;

        if(Net->TaskFlags & SBN_TASK_RECV)
        {
            continue;
        }/* end if */

        if(Net->IfOps->RecvFromNet)
        {
            int MsgCnt = 0;
            // TODO: make configurable
            for(MsgCnt = 0; MsgCnt < 100; MsgCnt++)
            {
                memset(Msg, 0, sizeof(Msg));

                Status = Net->IfOps->RecvFromNet(
                    Net, &MsgType, &MsgSz, &ProcessorID, Msg);

                if(Status == SBN_IF_EMPTY)
                {
                    break; /* no (more) messages */
                }/* end if */

                /* for UDP, the message received may not be from the peer
                 * expected.
                 */
                SBN_PeerInterface_t *Peer = SBN_GetPeer(Net, ProcessorID);

                if(Peer)
                {
                    OS_GetLocalTime(&Peer->LastRecv);
                }
                else
                {
                    CFE_EVS_SendEvent(SBN_PEERTASK_EID, CFE_EVS_ERROR,
                        "unknown peer (ProcessorID=%d)", ProcessorID);
                }/* end if */

                SBN_ProcessNetMsg(Net, MsgType, ProcessorID, MsgSz, Msg);
            }
        }
        else if(Net->IfOps->RecvFromPeer)
        {
            SBN_PeerIdx_t PeerIdx = 0;
            for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
            {
                SBN_PeerInterface_t *Peer = &Net->Peers[PeerIdx];

                /* Process up to 100 received messages
                 * TODO: make configurable
                 */
                int MsgCnt = 0;
                for(MsgCnt = 0; MsgCnt < 100; MsgCnt++)
                {
                    CFE_ProcessorID_t ProcessorID = 0;
                    SBN_MsgType_t MsgType = 0;
                    SBN_MsgSz_t MsgSz = 0;

                    memset(Msg, 0, sizeof(Msg));

                    Status = Net->IfOps->RecvFromPeer(Net, Peer,
                        &MsgType, &MsgSz, &ProcessorID, Msg);

                    if(Status == SBN_IF_EMPTY)
                    {
                        break; /* no (more) messages */
                    }/* end if */

                    OS_GetLocalTime(&Peer->LastRecv);

                    SBN_ProcessNetMsg(Net, MsgType, ProcessorID, MsgSz, Msg);
                }/* end for */
            }/* end for */
        }
        else
        {
            CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
                "neither RecvFromPeer nor RecvFromNet defined for net #%d",
                NetIdx);
        }/* end if */
    }/* end for */
}/* end SBN_RecvNetMsgs */

/**
 * Sends a message to a peer using the module's SendNetMsg.
 *
 * @param MsgType SBN type of the message
 * @param MsgSz Size of the message
 * @param Msg Message to send
 * @param Peer The peer to send the message to.
 * @return Number of characters sent on success, -1 on error.
 *
 */
SBN_MsgSz_t SBN_SendNetMsg(SBN_MsgType_t MsgType, SBN_MsgSz_t MsgSz, void *Msg, SBN_PeerInterface_t *Peer)
{
    SBN_MsgSz_t SentSz = 0;
    SBN_NetInterface_t *Net = Peer->Net;

    if(Peer->SendTaskID)
    {
        if(OS_MutSemTake(SBN.SendMutex) != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR, "unable to take mutex");
            return -1;
        }/* end if */
    }/* end if */

    SentSz = Net->IfOps->Send(Peer, MsgType, MsgSz, Msg);

    /* for clients that need a poll or heartbeat, update time even when failing */
    OS_GetLocalTime(&Peer->LastSend);

    if(SentSz != -1)
    {
        Peer->SendCnt++;
    }
    else
    {
        Peer->SendErrCnt++;
    }/* end if */

    if(Peer->SendTaskID)
    {
        if(OS_MutSemGive(SBN.SendMutex) != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR, "unable to give mutex");
            return -1;
        }/* end if */
    }/* end if */

    return SentSz;
}/* end SBN_SendNetMsg */

typedef struct
{
    SBN_NetIdx_t NetIdx;
    SBN_PeerIdx_t PeerIdx;
    OS_TaskID_t SendTaskID;
    CFE_SB_MsgPtr_t SBMsgPtr;
    CFE_SB_MsgId_t MsgID;
    SBN_NetInterface_t *Net;
    SBN_PeerInterface_t *Peer;
} SendTaskData_t;

/**
 * \brief When a peer is connected, a task is created to listen to the relevant
 * pipe for messages to send to that peer.
 */
static void SendTask(void)
{
    SendTaskData_t D;
    SBN_Filter_Ctx_t Filter_Context;

    Filter_Context.MyProcessorID = CFE_PSP_GetProcessorId();
    Filter_Context.MySpacecraftID = CFE_PSP_GetSpacecraftId();

    memset(&D, 0, sizeof(D));

    if(CFE_ES_RegisterChildTask() != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_PEERTASK_EID, CFE_EVS_ERROR, "unable to register child task");
        return;
    }/* end if */

    D.SendTaskID = OS_TaskGetId();

    for(D.NetIdx = 0; D.NetIdx < SBN.NetCnt; D.NetIdx++)
    {
        D.Net = &SBN.Nets[D.NetIdx];
        for(D.PeerIdx = 0; D.PeerIdx < D.Net->PeerCnt; D.PeerIdx++)
        {
            D.Peer = &D.Net->Peers[D.PeerIdx];
            if(D.Peer->SendTaskID == D.SendTaskID)
            {
                break;
            }/* end if */
        }/* end for */

        if(D.PeerIdx < D.Net->PeerCnt)
        {
            break; /* found a ringer */
        }/* end if */
    }/* end for */

    if(D.NetIdx == SBN.NetCnt)
    {
        CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
            "error connecting send task\n");
        return;
    }/* end if */

    while(1)
    {
        SBN_ModuleIdx_t FilterIdx = 0;

        if(!D.Peer->Connected)
        {
            OS_TaskDelay(SBN_MAIN_LOOP_DELAY);
            continue;
        }/* end if */

        if(CFE_SB_RcvMsg(&D.SBMsgPtr, D.Peer->Pipe, CFE_SB_PEND_FOREVER)
            != CFE_SUCCESS)
        {
            break;
        }/* end if */

        Filter_Context.PeerProcessorID = D.Peer->ProcessorID;
        Filter_Context.PeerSpacecraftID = D.Peer->SpacecraftID;

        for(FilterIdx = 0; FilterIdx < D.Peer->FilterCnt; FilterIdx++)
        {
            SBN_Status_t Status;

            if(D.Peer->Filters[FilterIdx]->FilterSend == NULL)
            {
                continue;
            }/* end if */
            
            Status = (D.Peer->Filters[FilterIdx]->FilterSend)(D.SBMsgPtr, &Filter_Context);
            if (Status == SBN_IF_EMPTY) /* filter requests not sending this msg, see below for loop */
            {
                break;
            }/* end if */

            if(Status != SBN_SUCCESS)
            {
                /* something fatal happened, exit */
                return;
            }/* end if */
        }/* end for */

        if(FilterIdx < D.Peer->FilterCnt)
        {
            /* one of the above filters suggested rejecting this message */
            continue;
        }/* end if */

        SBN_SendNetMsg(SBN_APP_MSG,
            CFE_SB_GetTotalMsgLength(D.SBMsgPtr),
            D.SBMsgPtr, D.Peer);
    }/* end while */
}/* end SendTask */

/**
 * Iterate through all peers, examining the pipe to see if there are messages
 * I need to send to that peer.
 */
static void CheckPeerPipes(void)
{
    int ReceivedFlag = 0, iter = 0;
    CFE_SB_MsgPtr_t SBMsgPtr = 0;
    SBN_Filter_Ctx_t Filter_Context;

    Filter_Context.MyProcessorID = CFE_PSP_GetProcessorId();
    Filter_Context.MySpacecraftID = CFE_PSP_GetSpacecraftId();

    /**
     * \note This processes one message per peer, then start again until no
     * peers have pending messages. At max only process SBN_MAX_MSG_PER_WAKEUP
     * per peer per wakeup otherwise I will starve other processing.
     */
    for(iter = 0; iter < SBN_MAX_MSG_PER_WAKEUP; iter++)
    {
        ReceivedFlag = 0;

        SBN_NetIdx_t NetIdx = 0;
        for(NetIdx = 0; NetIdx < SBN.NetCnt; NetIdx++)
        {
            SBN_NetInterface_t *Net = &SBN.Nets[NetIdx];

            if (Net->SendTaskID)
            {
                continue;
            }/* end if */

            SBN_PeerIdx_t PeerIdx = 0;
            for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
            {
                SBN_ModuleIdx_t FilterIdx = 0;
                SBN_PeerInterface_t *Peer = &Net->Peers[PeerIdx];

                /* if peer data is not in use, go to next peer */
                if(Peer->SendTaskID || Peer->Connected == 0 ||
                    CFE_SB_RcvMsg(&SBMsgPtr, Peer->Pipe, CFE_SB_POLL)
                        != CFE_SUCCESS)
                {
                    continue;
                }/* end if */

                ReceivedFlag = 1;

                Filter_Context.PeerProcessorID = Peer->ProcessorID;
                Filter_Context.PeerSpacecraftID = Peer->SpacecraftID;

                for(FilterIdx = 0; FilterIdx < Peer->FilterCnt; FilterIdx++)
                {
                    SBN_Status_t Status;
                    
                    if(Peer->Filters[FilterIdx]->FilterSend == NULL)
                    {
                        continue;
                    }/* end if */
                    
                    Status = (Peer->Filters[FilterIdx]->FilterSend)(SBMsgPtr, &Filter_Context);

                    if (Status == SBN_IF_EMPTY) /* filter requests not sending this msg, see below for loop */
                    {
                        break;
                    }/* end if */

                    if(Status != SBN_SUCCESS)
                    {
                        /* something fatal happened, exit */
                        return;
                    }/* end if */
                }/* end for */

                if(FilterIdx < Peer->FilterCnt)
                {
                    /* one of the above filters suggested rejecting this message */
                    continue;
                }/* end if */

                SBN_SendNetMsg(SBN_APP_MSG,
                    CFE_SB_GetTotalMsgLength(SBMsgPtr),
                    SBMsgPtr, Peer);
            }/* end for */
        }/* end for */

        if(!ReceivedFlag)
        {
            break;
        }/* end if */
    } /* end for */
}/* end CheckPeerPipes */

/**
 * Iterate through all peers, calling the poll interface if no messages have
 * been sent in the last SBN_POLL_TIME seconds.
 */
static void PeerPoll(void)
{
    SBN_NetIdx_t NetIdx = 0;
    for(NetIdx = 0; NetIdx < SBN.NetCnt; NetIdx++)
    {
        SBN_NetInterface_t *Net = &SBN.Nets[NetIdx];

        if(Net->RecvTaskID)
        {
            continue;
        }/* end if */

        SBN_PeerIdx_t PeerIdx = 0;
        for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
        {
            SBN_PeerInterface_t *Peer = &Net->Peers[PeerIdx];

            if(Net->RecvTaskID)
            {
                continue;
            }/* end if */

            Net->IfOps->PollPeer(Peer);
        }/* end for */
    }/* end for */
}/* end PeerPoll */

/**
 * Loops through all hosts and peers, initializing all.
 *
 * @return SBN_SUCCESS if interface is initialized successfully
 *         SBN_ERROR otherwise
 */
static SBN_Status_t InitInterfaces(void)
{
    if(SBN.NetCnt < 1)
    {
        CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
            "no networks configured");

        return SBN_ERROR;
    }/* end if */

    SBN_NetIdx_t NetIdx = 0;
    for(NetIdx = 0; NetIdx < SBN.NetCnt; NetIdx++)
    {
        SBN_NetInterface_t *Net = &SBN.Nets[NetIdx];

        if(!Net->Configured)
        {
            CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
                "network #%d not configured", NetIdx);

            return SBN_ERROR;
        }/* end if */

        Net->IfOps->InitNet(Net);

        SBN_Status_t Status = SBN_SUCCESS;

        if(Net->IfOps->RecvFromNet && Net->TaskFlags & SBN_TASK_RECV)
        {
            char RecvTaskName[32];
            snprintf(RecvTaskName, OS_MAX_API_NAME, "sbn_recvs_%d", NetIdx);
            Status = CFE_ES_CreateChildTask(&(Net->RecvTaskID),
                RecvTaskName, (CFE_ES_ChildTaskMainFuncPtr_t)&RecvNetTask,
                NULL, CFE_PLATFORM_ES_DEFAULT_STACK_SIZE + 2 * sizeof(RecvNetTaskData_t),
                0, 0);

            if(Status != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
                    "error creating task for net %d", NetIdx);
                return Status;
            }/* end if */
        }/* end if */

        SBN_PeerIdx_t PeerIdx = 0;
        for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
        {
            SBN_PeerInterface_t *Peer = &Net->Peers[PeerIdx];

            Net->IfOps->InitPeer(Peer);

            if(Net->IfOps->RecvFromPeer && Peer->TaskFlags & SBN_TASK_RECV)
            {
                char RecvTaskName[32];
                snprintf(RecvTaskName, OS_MAX_API_NAME, "sbn_recv_%d", PeerIdx);
                Status = CFE_ES_CreateChildTask(&(Peer->RecvTaskID),
                    RecvTaskName, (CFE_ES_ChildTaskMainFuncPtr_t)&RecvPeerTask,
                    NULL,
                    CFE_PLATFORM_ES_DEFAULT_STACK_SIZE + 2 * sizeof(RecvPeerTaskData_t),
                    0, 0);
                /* TODO: more accurate stack size required */

                if(Status != CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
                        "error creating task for %d", Peer->ProcessorID);
                    return Status;
                }/* end if */
            }/* end if */

            if(Peer->TaskFlags & SBN_TASK_SEND)
            {
                char SendTaskName[32];

                snprintf(SendTaskName, 32, "sendT_%d_%d", NetIdx,
                    Peer->ProcessorID);
                Status = CFE_ES_CreateChildTask(&(Peer->SendTaskID),
                    SendTaskName, (CFE_ES_ChildTaskMainFuncPtr_t)&SendTask, NULL,
                    CFE_PLATFORM_ES_DEFAULT_STACK_SIZE + 2 * sizeof(SendTaskData_t), 0, 0);

                if(Status != CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
                        "error creating send task for %d", Peer->ProcessorID);
                    return Status;
                }/* end if */
            }/* end if */
        }/* end for */
    }/* end for */

    CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_INFORMATION,
        "configured, %d nets",
        SBN.NetCnt);

    for(NetIdx = 0; NetIdx < SBN.NetCnt; NetIdx++)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_INFORMATION,
            "net %d has %d peers", NetIdx, SBN.Nets[NetIdx].PeerCnt);
    }/* end for */

    return SBN_SUCCESS;
}/* end InitInterfaces */

/**
 * This function waits for the scheduler (SCH) to wake this code up, so that
 * nothing transpires until the cFE is fully operational.
 *
 * @param[in] iTimeOut The time to wait for the scheduler to notify this code.
 * @return CFE_SUCCESS on success, otherwise an error value.
 */
static CFE_Status_t WaitForWakeup(int32 iTimeOut)
{
    CFE_Status_t Status = CFE_SUCCESS;
    CFE_SB_MsgPtr_t Msg = 0;

    /* Wait for WakeUp messages from scheduler */
    Status = CFE_SB_RcvMsg(&Msg, SBN.CmdPipe, iTimeOut);

    switch(Status)
    {
        case CFE_SB_NO_MESSAGE:
        case CFE_SB_TIME_OUT:
            Status = CFE_SUCCESS;
            break;
        case CFE_SUCCESS:
            SBN_HandleCommand(Msg);
            break;
        default:
            return Status;
    }/* end switch */

    /* For sbn, we still want to perform cyclic processing
    ** if the WaitForWakeup time out
    ** cyclic processing at timeout rate
    */
    CFE_ES_PerfLogEntry(SBN_PERF_RECV_ID);

    SBN_RecvNetMsgs();

    SBN_CheckSubscriptionPipe();

    CheckPeerPipes();

    PeerPoll();

    if(Status == CFE_SB_NO_MESSAGE) Status = CFE_SUCCESS;

    CFE_ES_PerfLogExit(SBN_PERF_RECV_ID);

    return Status;
}/* end WaitForWakeup */

/**
 * Waits for either a response to the "get subscriptions" message from SB, OR
 * an event message that says SB has finished initializing. The latter message
 * means that SB was not started at the time SBN sent the "get subscriptions"
 * message, so that message will need to be sent again.
 *
 * @return SBN_SUCCESS or SBN_ERROR
 */
static SBN_Status_t WaitForSBStartup(void)
{
    CFE_EVS_Packet_t *EvsTlm = NULL;
    CFE_SB_MsgPtr_t SBMsgPtr = 0;
    uint8 counter = 0;
    CFE_SB_PipeId_t EventPipe = 0;
    CFE_Status_t Status = SBN_SUCCESS;

    /* Create event message pipe */
    Status = CFE_SB_CreatePipe(&EventPipe, 20, "SBNEventPipe");
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "failed to create event pipe (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */

    /* Subscribe to event messages temporarily to be notified when SB is done
     * initializing
     */
    Status = CFE_SB_Subscribe(CFE_EVS_EVENT_MSG_MID, EventPipe);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "failed to subscribe to event pipe (%d)", (int)Status);
        return SBN_ERROR;
    }/* end if */

    while(1)
    {
        /* Check for subscription message from SB */
        if(SBN_CheckSubscriptionPipe())
        {
            /* SBN does not need to re-send request messages to SB */
            break;
        }
        else if(counter % 100 == 0)
        {
            /* Send subscription request messages again. This may cause the SB
             * to respond to duplicate requests but that should be okay
             */
            SBN_SendSubsRequests();
        }/* end if */

        /* Check for event message from SB */
        if(CFE_SB_RcvMsg(&SBMsgPtr, EventPipe, 100) == CFE_SUCCESS)
        {
            if(CFE_SB_GetMsgId(SBMsgPtr) == CFE_EVS_EVENT_MSG_MID)
            {
                EvsTlm = (CFE_EVS_Packet_t *)SBMsgPtr;

                /* If it's an event message from SB, make sure it's the init
                 * message
                 */
                if(strcmp(EvsTlm->Payload.PacketID.AppName, "CFE_SB") == 0
                    && EvsTlm->Payload.PacketID.EventID == CFE_SB_INIT_EID)
                {
                    break;
                }/* end if */
            }/* end if */
        }/* end if */

        counter++;
    }/* end while */

    /* Unsubscribe from event messages */
    if(CFE_SB_Unsubscribe(CFE_EVS_EVENT_MSG_MID, EventPipe) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "unable to unsubscribe from event messages");
    }/* end if */

    if(CFE_SB_DeletePipe(EventPipe) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "unable to delete event pipe");
    }/* end if */

    /* SBN needs to re-send request messages */
    return SBN_SUCCESS;
}/* end WaitForSBStartup */

static int32 ConfTblVal(void *TblPtr)
{
    /** TODO: write */
    return CFE_SUCCESS;
}/* end ConfTblVal */

static cpuaddr LoadConf_Module(SBN_Module_Entry_t *e, CFE_ES_ModuleID_t *ModuleIDPtr)
{
    cpuaddr StructAddr;

    CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_INFORMATION, "linking symbol (%s)", e->LibSymbol);
    if(OS_SymbolLookup(&StructAddr, e->LibSymbol) != OS_SUCCESS) /* try loading it if it's not already loaded */
    {
        if(e->LibFileName[0] == '\0')
        {
            CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_ERROR,
                "invalid module (Name=%s)", e->Name);
            return 0;
        }

        CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_INFORMATION,
            "loading module (Name=%s, File=%s)", e->Name, e->LibFileName);
        if(OS_ModuleLoad(ModuleIDPtr, e->Name, e->LibFileName) != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_ERROR,
                "invalid module file (Name=%s LibFileName=%s)", e->Name,
                e->LibFileName);
            return 0;
        }/* end if */

        CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_INFORMATION,
            "trying symbol again (%s)", e->LibSymbol);
        if(OS_SymbolLookup(&StructAddr, e->LibSymbol) != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_ERROR,
                "invalid symbol (Name=%s LibSymbol=%s)", e->Name,
                e->LibSymbol);
            return 0;
        }
    }/* end if */

    return StructAddr;
}/* end LoadConf_Module */

/**
 * Load the filters from the table.
 * @param FilterModules[in] - The filter module entries in the table.
 * @param FilterModuleCnt[in] - The number of entries in FilterModules.
 * @param ModuleNames[in] - The array of filters this peer/net wishes to use.
 * @param FilterFns[out] - The function pointers for the filters requested.
 * @return The number of entries in FilterFns.
 */
static SBN_ModuleIdx_t LoadConf_Filters(SBN_Module_Entry_t *FilterModules,
    SBN_ModuleIdx_t FilterModuleCnt, SBN_FilterInterface_t **ConfFilters,
    const char ModuleNames[SBN_MAX_FILTERS_PER_PEER][SBN_MAX_MOD_NAME_LEN],
    SBN_FilterInterface_t **Filters)
{
    int i = 0;
    SBN_ModuleIdx_t FilterCnt = 0;

    memset(FilterModules, 0, sizeof(*FilterModules) * FilterCnt);

    for (i = 0; *ModuleNames[i] && i < SBN_MAX_FILTERS_PER_PEER; i++)
    {
        SBN_ModuleIdx_t FilterIdx = 0;
        for (FilterIdx = 0; FilterIdx < FilterModuleCnt; FilterIdx++)
        {
            if(strcmp(ModuleNames[i], FilterModules[FilterIdx].Name) == 0)
            {
                break;
            }/* end if */
        }/* end for */

        if(FilterIdx == FilterModuleCnt)
        {
            CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_ERROR, "Invalid filter name: %s", ModuleNames[i]);
            continue;
        }/* end if */

        Filters[FilterCnt++] = ConfFilters[FilterIdx];
    }/* end for */

    return FilterCnt;
}/* end LoadConf_Filters() */

static SBN_Status_t LoadConf(void)
{
    SBN_ConfTbl_t *TblPtr = NULL;
    SBN_ModuleIdx_t ModuleIdx = 0;
    SBN_PeerIdx_t PeerIdx = 0;
    SBN_FilterInterface_t *Filters[SBN_MAX_MOD_CNT];

    memset(Filters, 0, sizeof(Filters));

    if(CFE_TBL_GetAddress((void **)&TblPtr, SBN.ConfTblHandle) != CFE_TBL_INFO_UPDATED)
    {
        CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_ERROR,
            "unable to get conf table address");
        CFE_TBL_Unregister(SBN.ConfTblHandle);
        return SBN_ERROR;
    }/* end if */

    /* load protocol modules */
    for(ModuleIdx = 0; ModuleIdx < TblPtr->ProtocolCnt; ModuleIdx++)
    {
        CFE_ES_ModuleID_t ModuleID = 0;

        SBN_IfOps_t *Ops = (SBN_IfOps_t *)LoadConf_Module(
            &TblPtr->ProtocolModules[ModuleIdx], &ModuleID);

        if (Ops == NULL)
        {
            /* LoadConf_Module already generated an event */
            return SBN_ERROR;
        }/* end if */

        CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_INFORMATION, "calling init fn");
        if(Ops->InitModule(SBN_PROTOCOL_VERSION, TblPtr->ProtocolModules[ModuleIdx].BaseEID) != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_ERROR, "error in init");
            return SBN_ERROR;
        }/* end if */

        SBN.IfOps[ModuleIdx] = Ops;
        SBN.ProtocolModules[ModuleIdx] = ModuleID;
    }/* end for */

    /* load filter modules */
    for(ModuleIdx = 0; ModuleIdx < TblPtr->FilterCnt; ModuleIdx++)
    {
        CFE_ES_ModuleID_t ModuleID = 0;

        Filters[ModuleIdx] = (SBN_FilterInterface_t *)LoadConf_Module(
            &TblPtr->FilterModules[ModuleIdx], &ModuleID);

        if(Filters[ModuleIdx] == NULL)
        {
            /* LoadConf_Module already generated an event */
            return SBN_ERROR;
        }/* end if */

        if(Filters[ModuleIdx]->InitModule(SBN_FILTER_VERSION, TblPtr->FilterModules[ModuleIdx].BaseEID) != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_ERROR, "error in init");
            return SBN_ERROR;
        }/* end if */

        SBN.FilterModules[ModuleIdx] = ModuleID;
    }/* end for */

    /* load nets and peers */
    for(PeerIdx = 0; PeerIdx < TblPtr->PeerCnt; PeerIdx++)
    {
        SBN_Peer_Entry_t *e = &TblPtr->Peers[PeerIdx];

        for(ModuleIdx = 0; ModuleIdx < TblPtr->ProtocolCnt; ModuleIdx++)
        {
            if(strcmp(TblPtr->ProtocolModules[ModuleIdx].Name, e->ProtocolName) == 0)
            {
                break;
            }
        }
        
        if(ModuleIdx == TblPtr->ProtocolCnt)
        {
            CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_CRITICAL,
                "invalid module idx (peeridx=%d, modname=%s)", PeerIdx, e->ProtocolName);
            return SBN_ERROR;
        }/* end if */

        if(e->NetNum < 0 || e->NetNum >= SBN_MAX_NETS)
        {
            CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_CRITICAL,
                "too many networks.");
            return SBN_ERROR;
        }/* end if */

        if(e->NetNum + 1 > SBN.NetCnt)
        {
            SBN.NetCnt = e->NetNum + 1;
        }/* end if */

        SBN_NetInterface_t *Net = &SBN.Nets[e->NetNum];
        if(e->ProcessorID == CFE_PSP_GetProcessorId() && e->SpacecraftID == CFE_PSP_GetSpacecraftId())
        {
            Net->Configured = TRUE;
            Net->ProtocolIdx = ModuleIdx;
            Net->IfOps = SBN.IfOps[ModuleIdx];
            Net->IfOps->LoadNet(Net, (const char *)e->Address);

            Net->FilterCnt = LoadConf_Filters(TblPtr->FilterModules, TblPtr->FilterCnt,
                Filters, e->Filters, Net->Filters);

            Net->TaskFlags = e->TaskFlags;
        }
        else
        {
            if(Net->PeerCnt >= SBN_MAX_PEER_CNT)
            {
                CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_CRITICAL,
                    "too many peer entries (%d, max = %d)",
                    Net->PeerCnt, SBN_MAX_PEER_CNT);
                return SBN_ERROR;
            }/* end if */
            SBN_PeerInterface_t *Peer = &Net->Peers[Net->PeerCnt++];
            memset(Peer, 0, sizeof(*Peer));
            Peer->Net = Net;
            Peer->ProcessorID = e->ProcessorID;
            Peer->SpacecraftID = e->SpacecraftID;

            Peer->FilterCnt = LoadConf_Filters(TblPtr->FilterModules, TblPtr->FilterCnt,
                Filters, e->Filters, Peer->Filters);

            SBN.IfOps[ModuleIdx]->LoadPeer(Peer, (const char *)e->Address);

            Net->TaskFlags = Peer->TaskFlags;
        }/* end if */
    }/* end for */

    /* address only needed at load time, release */
    if(CFE_TBL_ReleaseAddress(SBN.ConfTblHandle) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_CRITICAL, "unable to release address of conf tbl");
        return SBN_ERROR;
    }/* end if */

    /* ...but we keep the handle so we can be notified of updates */
    return SBN_SUCCESS;
}/* end LoadConf() */

static uint32 UnloadConf(void)
{
    uint32 Status;

    int NetIdx = 0;
    for(NetIdx = 0; NetIdx < SBN.NetCnt; NetIdx++)
    {
        SBN_NetInterface_t *Net = &SBN.Nets[NetIdx];
        if((Status = Net->IfOps->UnloadNet(Net)) != SBN_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_CRITICAL,
                "unable to unload network %d", NetIdx);
            return Status;
        }/* end if */
    }/* end for */

    if((Status = UnloadModules()) != SBN_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_CRITICAL,
            "unable to unload modules");
        return Status;
    }/* end if */

    return SBN_SUCCESS;
}/* end if */

static uint32 LoadConfTbl(void)
{
    int32 Status = CFE_SUCCESS;

    if((Status = CFE_TBL_Register(&SBN.ConfTblHandle, "SBN_ConfTbl",
        sizeof(SBN_ConfTbl_t), CFE_TBL_OPT_DEFAULT, &ConfTblVal))
        != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_ERROR,
            "unable to register conf tbl handle");
        return Status;
    }/* end if */

    if((Status = CFE_TBL_Load(SBN.ConfTblHandle, CFE_TBL_SRC_FILE,
            SBN_CONF_TBL_FILENAME))
        != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_ERROR,
            "unable to load conf tbl %s", SBN_CONF_TBL_FILENAME);
        CFE_TBL_Unregister(SBN.ConfTblHandle);
        return Status;
    }/* end if */

    if((Status = CFE_TBL_Manage(SBN.ConfTblHandle)) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_ERROR,
            "unable to manage conf tbl");
        CFE_TBL_Unregister(SBN.ConfTblHandle);
        return Status;
    }/* end if */

    if((Status = CFE_TBL_NotifyByMessage(SBN.ConfTblHandle, SBN_CMD_MID,
        SBN_TBL_CC, 0)) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_TBL_EID, CFE_EVS_ERROR,
            "unable to set notifybymessage for conf tbl");
        CFE_TBL_Unregister(SBN.ConfTblHandle);
        return Status;
    }/* end if */

    return LoadConf();
}/* end LoadConfTbl() */

/** \brief SBN Main Routine */
void SBN_AppMain(void)
{
    CFE_ES_TaskInfo_t TaskInfo;
    uint32  Status = CFE_SUCCESS;
    uint32  RunStatus = CFE_ES_APP_RUN,
            AppID = 0;

    if(CFE_ES_RegisterApp() != CFE_SUCCESS) return;

    if(CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER != CFE_SUCCESS)) return;

    if(CFE_ES_GetAppID(&AppID) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_CRITICAL,
            "unable to get AppID");
        return;
    }

    SBN.AppID = AppID;

    /* load my TaskName so I can ignore messages I send out to SB */
    uint32 TskId = OS_TaskGetId();
    if ((Status = CFE_ES_GetTaskInfo(&TaskInfo, TskId)) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "SBN failed to get task info (%d)", Status);
        return;
    }/* end if */

    strncpy(SBN.App_FullName, (const char *)TaskInfo.TaskName, OS_MAX_API_NAME - 1);

    CFE_ES_WaitForStartupSync(10000);

    if ((Status = LoadConfTbl()) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_INFORMATION,
            "SBN failed to load SBN.ConfTblHandle (%d)", Status);
        SBN.ConfTblHandle = 0;
    }/* end if */

    /** Create mutex for send tasks */
    Status = OS_MutSemCreate(&(SBN.SendMutex), "sbn_send_mutex", 0);

    if(Status != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "error creating mutex for send tasks");
        return;
    }

    if(InitInterfaces() == SBN_ERROR)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "unable to initialize interfaces");
        return;
    }/* end if */

    /* Create pipe for subscribes and unsubscribes from SB */
    Status = CFE_SB_CreatePipe(&SBN.SubPipe, SBN_SUB_PIPE_DEPTH, "SBNSubPipe");
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "failed to create subscription pipe (Status=%d)", (int)Status);
        return;
    }/* end if */

    Status = CFE_SB_SubscribeLocal(CFE_SB_ALLSUBS_TLM_MID, SBN.SubPipe,
        SBN_MAX_ALLSUBS_PKTS_ON_PIPE);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "failed to subscribe to allsubs (Status=%d)", (int)Status);
        return;
    }/* end if */

    Status = CFE_SB_SubscribeLocal(CFE_SB_ONESUB_TLM_MID, SBN.SubPipe,
        SBN_MAX_ONESUB_PKTS_ON_PIPE);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "failed to subscribe to sub (Status=%d)", (int)Status);
        return;
    }/* end if */

    /* Create pipe for HK requests and gnd commands */
    /* TODO: make configurable depth */
    Status = CFE_SB_CreatePipe(&SBN.CmdPipe, 20, "SBNCmdPipe");
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "failed to create command pipe (%d)", (int)Status);
        return;
    }/* end if */

    Status = CFE_SB_Subscribe(SBN_CMD_MID, SBN.CmdPipe);
    if(Status == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_INFORMATION,
            "Subscribed to command MID 0x%04X", SBN_CMD_MID);
    }
    else
    {
        CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_ERROR,
            "failed to subscribe to command pipe (%d)", (int)Status);
        return;
    }/* end if */

    CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_INFORMATION,
        "initialized (CFE_CPU_NAME='%s' ProcessorID=%d SpacecraftId=%d %s "
        "SBN.AppID=%d...",
        CFE_CPU_NAME, CFE_PSP_GetProcessorId(), CFE_PSP_GetSpacecraftId(),
#ifdef SOFTWARE_BIG_BIT_ORDER
        "big-endian",
#else /* !SOFTWARE_BIG_BIT_ORDER */
        "little-endian",
#endif /* SOFTWARE_BIG_BIT_ORDER */
        (int)SBN.AppID);
    CFE_EVS_SendEvent(SBN_INIT_EID, CFE_EVS_INFORMATION,
        "...SBN_IDENT=%s CMD_MID=0x%04X)",
        SBN_IDENT,
        SBN_CMD_MID
    );

    SBN_InitializeCounters();

    /* Wait for event from SB saying it is initialized OR a response from SB
       to the above messages. TRUE means it needs to re-send subscription
       requests */
    if(WaitForSBStartup()) SBN_SendSubsRequests();

    if(Status != CFE_SUCCESS) RunStatus = CFE_ES_APP_ERROR;

    /* Loop Forever */
    while(CFE_ES_RunLoop(&RunStatus))
    {
        WaitForWakeup(SBN_MAIN_LOOP_DELAY);
    }/* end while */

    int NetIdx = 0;
    for(NetIdx = 0; NetIdx < SBN.NetCnt; NetIdx++)
    {
        SBN_NetInterface_t *Net = &SBN.Nets[NetIdx];
        Net->IfOps->UnloadNet(Net);
    }/* end for */

    UnloadModules();

    CFE_ES_ExitApp(RunStatus);
}/* end SBN_AppMain */

/**
 * Sends a message to a peer.
 * @param[in] MsgType The type of the message (application data, SBN protocol)
 * @param[in] ProcessorID The ProcessorID to send this message to.
 * @param[in] MsgSz The size of the message (in bytes).
 * @param[in] Msg The message contents.
 */
void SBN_ProcessNetMsg(SBN_NetInterface_t *Net, SBN_MsgType_t MsgType,
    CFE_ProcessorID_t ProcessorID, SBN_MsgSz_t MsgSize, void *Msg)
{
    int Status = 0;
    SBN_PeerInterface_t *Peer = SBN_GetPeer(Net, ProcessorID);

    if(!Peer)
    {
        CFE_EVS_SendEvent(SBN_PEERTASK_EID, CFE_EVS_ERROR,
            "unknown peer (ProcessorID=%d)", ProcessorID);
        return;
    }/* end if */

    switch(MsgType)
    {
    	case SBN_PROTO_MSG:
        {
            uint8 Ver = ((uint8 *)Msg)[0];
            if(Ver != SBN_PROTO_VER)
            {
                CFE_EVS_SendEvent(SBN_SB_EID, CFE_EVS_ERROR,
                    "SBN protocol version mismatch with ProcessorID %d, "
                        "my version=%d, peer version %d",
                    (int)Peer->ProcessorID, (int)SBN_PROTO_VER, (int)Ver);
            }
            else
            {
                CFE_EVS_SendEvent(SBN_SB_EID, CFE_EVS_INFORMATION,
                    "SBN protocol version match with ProcessorID %d",
                    (int)Peer->ProcessorID);
            }/* end if */
            break;
        }/* end case */
        case SBN_APP_MSG:
        {
            SBN_ModuleIdx_t FilterIdx = 0;
            SBN_Filter_Ctx_t Filter_Context;

            Filter_Context.MyProcessorID = CFE_PSP_GetProcessorId();
            Filter_Context.MySpacecraftID = CFE_PSP_GetSpacecraftId();
            Filter_Context.PeerProcessorID = Peer->ProcessorID;
            Filter_Context.PeerSpacecraftID = Peer->SpacecraftID;

            for(FilterIdx = 0; FilterIdx < Peer->FilterCnt; FilterIdx++)
            {
                SBN_Status_t Status;

                if(Peer->Filters[FilterIdx]->FilterRecv == NULL)
                {
                    continue;
                }/* end if */

                Status = (Peer->Filters[FilterIdx]->FilterRecv)(Msg, &Filter_Context);

                if (Status == SBN_IF_EMPTY) /* filter requests not sending this msg, see below for loop */
                {
                    break;
                }/* end if */

                if(Status != SBN_SUCCESS)
                {
                    /* something fatal happened, exit */
                    return;
                }/* end if */
            }/* end for */

            if(FilterIdx < Peer->FilterCnt)
            {
                /* one of the above filters suggested rejecting this message */
                break;
            }/* end if */

            Status = CFE_SB_PassMsg(Msg);

            if(Status != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(SBN_SB_EID, CFE_EVS_ERROR,
                    "CFE_SB_PassMsg error (Status=%d MsgType=0x%x)",
                    Status, MsgType);
            }/* end if */
            break;
        }/* end case */
        case SBN_SUB_MSG:
            SBN_ProcessSubsFromPeer(Peer, Msg);
            break;

        case SBN_UNSUB_MSG:
            SBN_ProcessUnsubsFromPeer(Peer, Msg);
            break;

        case SBN_NO_MSG:
        default:
            /* Should I generate an event? Probably... */
            break;
    }/* end switch */
}/* end SBN_ProcessNetMsg */

/**
 * Find the PeerIndex for a given ProcessorID and net.
 * @param[in] Net The network interface to search.
 * @param[in] ProcessorID The ProcessorID of the peer being sought.
 * @return The Peer interface pointer, or NULL if not found.
 */
SBN_PeerInterface_t *SBN_GetPeer(SBN_NetInterface_t *Net, CFE_ProcessorID_t ProcessorID)
{
    SBN_PeerIdx_t PeerIdx = 0;

    for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
    {
        if(Net->Peers[PeerIdx].ProcessorID == ProcessorID)
        {
            return &Net->Peers[PeerIdx];
        }/* end if */
    }/* end for */

    return NULL;
}/* end SBN_GetPeer */

uint32 SBN_Connected(SBN_PeerInterface_t *Peer)
{
    if (Peer->Connected != 0)
    {
        CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
            "CPU %d already connected", Peer->ProcessorID);
        return SBN_ERROR;
    }/* end if */

    char PipeName[OS_MAX_API_NAME];

    /* create a pipe name string similar to SBN_0_Pipe */
    snprintf(PipeName, OS_MAX_API_NAME, "SBN_%d_Pipe", Peer->ProcessorID);
    int Status = CFE_SB_CreatePipe(&(Peer->Pipe), SBN_PEER_PIPE_DEPTH,
        PipeName);

    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
            "failed to create pipe '%s'", PipeName);

        return Status;
    }/* end if */

    CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_INFORMATION,
        "pipe created '%s'", PipeName);

    Status = CFE_SB_SetPipeOpts(Peer->Pipe, CFE_SB_PIPEOPTS_IGNOREMINE);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
            "failed to set pipe options '%s'", PipeName);

        return Status;
    }/* end if */

    CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_INFORMATION,
        "CPU %d connected", Peer->ProcessorID);

    uint8 ProtocolVer = SBN_PROTO_VER;
    SBN_SendNetMsg(SBN_PROTO_MSG, sizeof(ProtocolVer), &ProtocolVer, Peer);

    /* set this to current time so we don't think we've already timed out */
    OS_GetLocalTime(&Peer->LastRecv);

    SBN_SendLocalSubsToPeer(Peer);

    Peer->Connected = 1;

    return SBN_SUCCESS;
} /* end SBN_Connected() */

SBN_Status_t SBN_Disconnected(SBN_PeerInterface_t *Peer)
{
    if(Peer->Connected == 0)
    {
        CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_ERROR,
            "CPU %d not connected", Peer->ProcessorID);

        return SBN_ERROR;
    }

    Peer->Connected = 0; /**< mark as disconnected before deleting pipe */

    CFE_SB_DeletePipe(Peer->Pipe);
    Peer->Pipe = 0;

    Peer->SubCnt = 0; /* reset sub count, in case this is a reconnection */

    CFE_EVS_SendEvent(SBN_PEER_EID, CFE_EVS_INFORMATION,
        "CPU %d disconnected", Peer->ProcessorID);

    return SBN_SUCCESS;
}/* end SBN_Disconnected() */

SBN_Status_t SBN_ReloadConfTbl(void)
{
    SBN_Status_t Status;

    if((Status = UnloadConf()) != SBN_SUCCESS)
    {
        return Status;
    }/* end if */

    if(CFE_TBL_Update(SBN.ConfTblHandle) != CFE_SUCCESS)
    {
        return SBN_ERROR;
    }/* end if */

    return LoadConf();
}/* end SBN_ReloadConfTbl() */
