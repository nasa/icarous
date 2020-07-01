#include "sbn_udp_events.h"
#include "sbn_platform_cfg.h"
#include <network_includes.h>
#include <string.h>
#include <errno.h>

#include "sbn_interfaces.h"
#include "cfe.h"

CFE_EVS_EventID_t SBN_UDP_FIRST_EID;

/**
 * UDP-specific message types.
 */
#define SBN_UDP_HEARTBEAT_MSG   0xA0
#define SBN_UDP_ANNOUNCE_MSG    0xA1
#define SBN_UDP_DISCONN_MSG     0xA2

/**
 * \brief Number of seconds since last I've sent the peer a message when
 * I send an empty heartbeat message.
 */
#define SBN_UDP_PEER_HEARTBEAT 5

/**
 * \brief Number of seconds since I've last heard from the peer when I consider
 * the peer connection to be dropped.
 */
#define SBN_UDP_PEER_TIMEOUT 10

/**
 * \brief If we're not connected, send peer occasional messages to wake
 * them up and tell them "I'm here".
 */
#define SBN_UDP_ANNOUNCE_TIMEOUT 10

typedef struct
{
    OS_SockAddr_t Addr;
} SBN_UDP_Peer_t;

typedef struct
{
    OS_SockAddr_t Addr;
    uint32 Socket;
} SBN_UDP_Net_t;

static CFE_Status_t Init(int Version, CFE_EVS_EventID_t BaseEID)
{
    SBN_UDP_FIRST_EID = BaseEID;

    if(Version != 1) /* TODO: define */
    {
        OS_printf("SBN_UDP version mismatch: expected %d, got %d\n", 1, Version);
        return CFE_ES_ERR_APP_CREATE;
    }/* end if */

    OS_printf("SBN_UDP Lib Initialized.\n");
    return CFE_SUCCESS;
}/* end Init() */

static SBN_Status_t ConfAddr(OS_SockAddr_t *Addr, const char *Address)
{
    SBN_Status_t Status = SBN_SUCCESS;

    char AddrHost[OS_MAX_API_NAME];

    char *Colon = strchr(Address, ':');
    int ColonLen = 0;

    if(!Colon || (ColonLen = Colon - Address) >= OS_MAX_API_NAME)
    {
        CFE_EVS_SendEvent(SBN_UDP_CONFIG_EID, CFE_EVS_ERROR,
                "invalid address (Address=%s)", Address);
        return SBN_ERROR;
    }/* end if */

    strncpy(AddrHost, Address, ColonLen);

    char *ValidatePtr = NULL;
    long Port = strtol(Colon + 1, &ValidatePtr, 0);
    if(!ValidatePtr || ValidatePtr == Colon + 1)
    {
        CFE_EVS_SendEvent(SBN_UDP_CONFIG_EID, CFE_EVS_ERROR,
                "invalid port (Address=%s)", Address);
    }/* end if */

    if((Status = OS_SocketAddrInit(Addr, OS_SocketDomain_INET)) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_UDP_SOCK_EID, CFE_EVS_ERROR,
            "addr init failed (Status=%d)", Status);
        return SBN_ERROR;
    }/* end if */

    if((Status = OS_SocketAddrFromString(Addr, AddrHost)) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_UDP_SOCK_EID, CFE_EVS_ERROR,
            "addr host set failed (AddrHost=%s, Status=%d)", AddrHost, Status);
        return SBN_ERROR;
    }/* end if */

    if((Status = OS_SocketAddrSetPort(Addr, Port)) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_UDP_SOCK_EID, CFE_EVS_ERROR,
            "addr port set failed (Port=%ld, Status=%d)", Port, Status);
        return SBN_ERROR;
    }/* end if */

    return SBN_SUCCESS;
}/* end ConfAddr() */

static SBN_Status_t LoadNet(SBN_NetInterface_t *Net, const char *Address)
{
    SBN_UDP_Net_t *NetData = (SBN_UDP_Net_t *)Net->ModulePvt;

    CFE_EVS_SendEvent(SBN_UDP_CONFIG_EID, CFE_EVS_INFORMATION,
        "configuring net (NetData=0x%lx, Address=%s)", (long unsigned int)NetData, Address);

    SBN_Status_t Status = ConfAddr(&NetData->Addr, Address);

    if(Status == SBN_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_UDP_CONFIG_EID, CFE_EVS_INFORMATION,
            "configured (NetData=0x%lx)", (long unsigned int)NetData);
    }/* end if */

    return Status;
}/* end LoadNet */

static SBN_Status_t LoadPeer(SBN_PeerInterface_t *Peer, const char *Address)
{
    SBN_UDP_Peer_t *PeerData = (SBN_UDP_Peer_t *)Peer->ModulePvt;

    CFE_EVS_SendEvent(SBN_UDP_CONFIG_EID, CFE_EVS_INFORMATION,
        "configuring peer (PeerData=0x%lx, Address=%s)", (long unsigned int)PeerData, Address);

    SBN_Status_t Status = ConfAddr(&PeerData->Addr, Address);

    if(Status == SBN_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_UDP_CONFIG_EID, CFE_EVS_INFORMATION,
            "configured (PeerData=0x%lx)", (long unsigned int)PeerData);
    }/* end if */

    return Status;
}/* end LoadPeer() */

/**
 * Initializes an UDP host.
 *
 * @param  Interface data structure containing the file entry
 * @return SBN_SUCCESS on success, error code otherwise
 */
static SBN_Status_t InitNet(SBN_NetInterface_t *Net)
{
    SBN_UDP_Net_t *NetData = (SBN_UDP_Net_t *)Net->ModulePvt;

    CFE_EVS_SendEvent(SBN_UDP_SOCK_EID, CFE_EVS_DEBUG,
        "creating socket (NetData=0x%lx)", (long unsigned int)NetData);

    if(OS_SocketOpen(&(NetData->Socket), OS_SocketDomain_INET, OS_SocketType_DATAGRAM) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_UDP_SOCK_EID, CFE_EVS_ERROR, "socket call failed");
        return SBN_ERROR;
    }/* end if */

    if(OS_SocketBind(NetData->Socket, &NetData->Addr) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_UDP_SOCK_EID, CFE_EVS_ERROR,
            "bind call failed (NetData=0x%lx, Socket=%d)",
            (long unsigned int)NetData, NetData->Socket);
        return SBN_ERROR;
    }/* end if */

    return SBN_SUCCESS;
}/* end InitNet() */

/**
 * Initializes an UDP host or peer data struct depending on the
 * CPU name.
 *
 * @param  Interface data structure containing the file entry
 * @return SBN_SUCCESS on success, error code otherwise
 */
static SBN_Status_t InitPeer(SBN_PeerInterface_t *Peer)
{
    return SBN_SUCCESS;
}/* end InitPeer() */

static SBN_Status_t PollPeer(SBN_PeerInterface_t *Peer)
{
    OS_time_t CurrentTime;
    OS_GetLocalTime(&CurrentTime);

    if(Peer->Connected)
    {
        if(CurrentTime.seconds - Peer->LastRecv.seconds
            > SBN_UDP_PEER_TIMEOUT)
        {
            CFE_EVS_SendEvent(SBN_UDP_DEBUG_EID, CFE_EVS_INFORMATION,
                "CPU %d disconnected", Peer->ProcessorID);

            SBN_Disconnected(Peer);
            return SBN_SUCCESS;
        }/* end if */

        if(CurrentTime.seconds - Peer->LastSend.seconds
            > SBN_UDP_PEER_HEARTBEAT)
        {
            CFE_EVS_SendEvent(SBN_UDP_DEBUG_EID, CFE_EVS_INFORMATION,
                "CPU %d - heartbeat", Peer->ProcessorID);
            return SBN_SendNetMsg(SBN_UDP_HEARTBEAT_MSG, 0, NULL, Peer);
        }/* end if */
    }
    else
    {
        if(Peer->ProcessorID < CFE_PSP_GetProcessorId() &&
            CurrentTime.seconds - Peer->LastSend.seconds
                > SBN_UDP_ANNOUNCE_TIMEOUT)
        {
            return SBN_SendNetMsg(SBN_UDP_ANNOUNCE_MSG, 0, NULL, Peer);
        }/* end if */
    }/* end if */

    return SBN_SUCCESS;
}/* end PollPeer() */

static SBN_MsgSz_t Send(SBN_PeerInterface_t *Peer, SBN_MsgType_t MsgType,
    SBN_MsgSz_t MsgSz, void *Payload)
{
    size_t BufSz = MsgSz + SBN_PACKED_HDR_SZ;
    uint8 Buf[BufSz];

    SBN_UDP_Peer_t *PeerData = (SBN_UDP_Peer_t *)Peer->ModulePvt;
    SBN_NetInterface_t *Net = Peer->Net;
    SBN_UDP_Net_t *NetData = (SBN_UDP_Net_t *)Net->ModulePvt;

    SBN_PackMsg(&Buf, MsgSz, MsgType, CFE_PSP_GetProcessorId(), Payload);

    OS_SockAddr_t Addr;
    if(OS_SocketAddrInit(&Addr, OS_SocketDomain_INET) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_UDP_SOCK_EID, CFE_EVS_ERROR, "socket addr init failed");
        return -1;
    }/* end if */

    OS_SocketSendTo(NetData->Socket, Buf, BufSz, &PeerData->Addr);

    return BufSz;
}/* end Send() */

/* Note that this Recv function is indescriminate, packets will be received
 * from all peers but that's ok, I just inject them into the SB and all is
 * good!
 */
static SBN_Status_t Recv(SBN_NetInterface_t *Net, SBN_MsgType_t *MsgTypePtr,
        SBN_MsgSz_t *MsgSzPtr, CFE_ProcessorID_t *ProcessorIDPtr,
        void *Payload)
{
    uint8 RecvBuf[SBN_MAX_PACKED_MSG_SZ];

    SBN_UDP_Net_t *NetData = (SBN_UDP_Net_t *)Net->ModulePvt;

    /* task-based peer connections block on reads, otherwise use select */
  
    uint32 StateFlags = OS_STREAM_STATE_READABLE;

    /* timeout returns an OS error */
    if(OS_SelectSingle(NetData->Socket, &StateFlags, 0) != OS_SUCCESS
        || !(StateFlags & OS_STREAM_STATE_READABLE))
    {
        /* nothing to receive */
        return SBN_IF_EMPTY;
    }/* end if */

    int Received = OS_SocketRecvFrom(NetData->Socket, (char *)&RecvBuf,
        CFE_SB_MAX_SB_MSG_SIZE, NULL, OS_PEND);

    if(Received < 0)
    {
        return SBN_ERROR;
    }/* end if */

    /* each UDP packet is a full SBN message */

    if(SBN_UnpackMsg(&RecvBuf, MsgSzPtr, MsgTypePtr, ProcessorIDPtr, Payload)
        == FALSE)
    {
        return SBN_ERROR;
    }/* end if */

    SBN_PeerInterface_t *Peer = SBN_GetPeer(Net, *ProcessorIDPtr);
    if(Peer == NULL)
    {
        return SBN_ERROR;
    }/* end if */

    if(!Peer->Connected)
    {
        SBN_Connected(Peer);
    }/* end if */

    if(*MsgTypePtr == SBN_UDP_DISCONN_MSG)
    {
        SBN_Disconnected(Peer);
    }

    return SBN_SUCCESS;
}/* end Recv() */

static SBN_Status_t UnloadPeer(SBN_PeerInterface_t *Peer)
{
    if(Peer->Connected)
    {
        CFE_EVS_SendEvent(SBN_UDP_DEBUG_EID, CFE_EVS_INFORMATION,
                "peer%d - sending disconnect", Peer->ProcessorID);
        SBN_SendNetMsg(SBN_UDP_DISCONN_MSG, 0, NULL, Peer);
        SBN_Disconnected(Peer);
    }/* end if */

    return SBN_SUCCESS;
}/* end UnloadPeer() */

static SBN_Status_t UnloadNet(SBN_NetInterface_t *Net)
{
    SBN_UDP_Net_t *NetData = (SBN_UDP_Net_t *)Net->ModulePvt;

    OS_close(NetData->Socket);

    SBN_PeerIdx_t PeerIdx = 0;
    for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
    {
        UnloadPeer(&Net->Peers[PeerIdx]);
    }/* end if */

    return SBN_SUCCESS;
}/* end UnloadNet() */

SBN_IfOps_t SBN_UDP_Ops =
{
    Init,
    LoadNet,
    LoadPeer,
    InitNet,
    InitPeer,
    PollPeer,
    Send,
    NULL,
    Recv,
    UnloadNet,
    UnloadPeer
};
