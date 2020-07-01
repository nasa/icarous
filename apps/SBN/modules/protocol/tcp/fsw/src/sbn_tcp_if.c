#include "sbn_interfaces.h"
#include "sbn_platform_cfg.h"
#include "cfe.h"
#include "sbn_tcp_events.h"

#include <network_includes.h>
#include <string.h>
#include <errno.h>

#define SBN_TCP_HEARTBEAT_MSG 0xA0

/**
 * If I haven't sent a message in SBN_TCP_PEER_HEARTBEAT seconds, send an empty
 * one just to maintain the connection. If this is set to 0, no heartbeat
 * messages will be generated.
 */
#define SBN_TCP_PEER_HEARTBEAT 5
/* #define SBN_TCP_PEER_HEARTBEAT 0 */

/**
 * If I haven't received a message from a peer in SBN_TCP_PEER_TIMEOUT seconds,
 * consider the peer lost and disconnect. If this is set to 0, no timeout is
 * checked.
 */
/* #define SBN_TCP_PEER_TIMEOUT 10 */
#define SBN_TCP_PEER_TIMEOUT 0

typedef struct
{
    bool InUse, ReceivingBody;
    int RecvSz;
    int Socket;
    uint8 BufNum;
    SBN_PeerInterface_t *PeerInterface; /* affiliated peer, if known */
} SBN_TCP_Conn_t;

typedef struct
{
    OS_SockAddr_t Addr;
    bool ConnectOut;
    uint8 BufNum; /* incoming buffer */
    OS_time_t LastConnectTry;
    SBN_TCP_Conn_t *Conn; /* when connected and affiliated */
} SBN_TCP_Peer_t;

typedef struct
{
    OS_SockAddr_t Addr;
    uint8 BufNum; /* outgoing buffer */
    int Socket; /* server socket */
    SBN_TCP_Conn_t Conns[SBN_MAX_PEER_CNT];
} SBN_TCP_Net_t;

CFE_EVS_EventID_t SBN_TCP_FIRST_EID = 0;

static CFE_Status_t Init(int Version, CFE_EVS_EventID_t EID)
{
    SBN_TCP_FIRST_EID = EID;
    if(Version != 1) /* TODO: define */
    {
        OS_printf("SBN_TCP version mismatch: expected %d, got %d\n", 1, Version);
        return CFE_ES_ERR_APP_CREATE;
    }/* end if */

    OS_printf("SBN_TCP Lib Initialized.\n");
    return CFE_SUCCESS;
}/* end Init() */

static SBN_Status_t ConfAddr(OS_SockAddr_t *Addr, const char *Address)
{
    char AddrHost[OS_MAX_API_NAME];
    int AddrLen;
    char *Colon = strchr(Address, ':');

    if(!Colon || (AddrLen = Colon - Address) >= OS_MAX_API_NAME)
    {
        CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_ERROR, "invalid net address");
        return SBN_ERROR;
    }/* end if */

    strncpy(AddrHost, Address, AddrLen);
    AddrHost[AddrLen] = '\0';
    char *ValidatePtr = NULL;

    OS_SocketPort_t Port = strtol(Colon + 1, &ValidatePtr, 0);

    if(!ValidatePtr || ValidatePtr == Colon + 1)
    {
        CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_ERROR, "invalid port");
        return SBN_ERROR;
    }/* end if */

    if(OS_SocketAddrInit(Addr, OS_SocketDomain_INET) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_ERROR, "socket addr init failed");
        return SBN_ERROR;
    }/* end if */

    if(OS_SocketAddrFromString(Addr, AddrHost) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_ERROR,
            "setting address host failed (AddrHost=%s)", AddrHost);
        return SBN_ERROR;
    }/* end if */
 
    if(OS_SocketAddrSetPort(Addr, Port) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_ERROR,
            "setting address port failed (Port=%d)", Port);
        return SBN_ERROR;
    }/* end if */

    return SBN_SUCCESS;
}/* end ConfAddr() */

static uint8 SendBufs[SBN_MAX_NETS][SBN_MAX_PACKED_MSG_SZ];
static int SendBufCnt = 0;

static SBN_TCP_Conn_t *NewConn(SBN_TCP_Net_t *NetData, int Socket)
{
    /* warning -- no protections against flooding */
    /* TODO: do I need a mutex? */

    int ConnID = 0;

    for (ConnID = 0; ConnID < SBN_MAX_PEER_CNT && NetData->Conns[ConnID].InUse; ConnID++);

    if(ConnID == SBN_MAX_PEER_CNT)
    {
        return NULL;
    }/* end if */

    SBN_TCP_Conn_t *Conn = &NetData->Conns[ConnID];

    memset(Conn, 0, sizeof(*Conn));

    Conn->Socket = Socket;

    Conn->InUse = TRUE;

    return Conn;
}/* end NewConn() */

static void Disconnected(SBN_PeerInterface_t *Peer)
{
    SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;
    SBN_TCP_Conn_t *Conn = PeerData->Conn;

    if(Conn)
    {
        OS_close(Conn->Socket);

        Conn->InUse = FALSE;
        PeerData->Conn = NULL;
    }/* end if */

    SBN_Disconnected(Peer);
}/* end Disconnected() */

static SBN_Status_t LoadNet(SBN_NetInterface_t *Net, const char *Address)
{
    SBN_TCP_Net_t *NetData = (SBN_TCP_Net_t *)Net->ModulePvt;

    CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_INFORMATION,
        "configuring net 0x%lx -> %s", (unsigned long int)NetData, Address);

    SBN_Status_t Status = ConfAddr(&NetData->Addr, Address);

    if (Status == SBN_SUCCESS)
    {
        NetData->BufNum = SendBufCnt++;

        CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_INFORMATION,
            "net 0x%lx configured", (unsigned long int)NetData);
    }/* end if */

    return Status;
}/* end LoadNet() */

static uint8 RecvBufs[SBN_MAX_PEER_CNT][SBN_MAX_PACKED_MSG_SZ];
static uint8 RecvBufCnt = 0;

static SBN_Status_t LoadPeer(SBN_PeerInterface_t *Peer, const char *Address)
{
    SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;

    CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_INFORMATION,
        "configuring peer 0x%lx -> %s", (unsigned long int)PeerData, Address);

    SBN_Status_t Status = ConfAddr(&PeerData->Addr, Address);

    if (Status == SBN_SUCCESS)
    {
        PeerData->BufNum = RecvBufCnt++;

        CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_INFORMATION,
            "peer 0x%lx configured", (unsigned long int)PeerData);
    }/* end if */

    return Status;
}/* end LoadPeer() */

/**
 * Initializes an TCP host or peer data struct depending on the
 * CPU name.
 *
 * @param  Interface data structure containing the file entry
 * @return SBN_SUCCESS on success, error code otherwise
 */
static SBN_Status_t InitNet(SBN_NetInterface_t *Net)
{
    SBN_TCP_Net_t *NetData = (SBN_TCP_Net_t *)Net->ModulePvt;

    OS_SocketID_t Socket = 0;

    if(OS_SocketOpen(&Socket, OS_SocketDomain_INET, OS_SocketType_STREAM) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_ERROR,
            "unable to create socket");
        return SBN_ERROR;
    }/* end if */

    if(OS_SocketBind(Socket, &NetData->Addr) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_ERROR,
            "bind call failed (0x%lx Socket=%d)",
            (unsigned long int)NetData, NetData->Socket);
        return SBN_ERROR;
    }

    NetData->Socket = Socket;

    return SBN_SUCCESS;
}/* end InitNet() */

/**
 * Initializes an TCP peer.
 *
 * @param  Interface data structure containing the file entry
 * @return SBN_SUCCESS on success, error code otherwise
 */
static SBN_Status_t InitPeer(SBN_PeerInterface_t *Peer)
{
    SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;

    PeerData->ConnectOut = (Peer->ProcessorID > CFE_PSP_GetProcessorId());

    return SBN_SUCCESS;
}/* end InitPeer() */

static void CheckNet(SBN_NetInterface_t *Net)
{
    CFE_Status_t Status = CFE_SUCCESS;
    SBN_TCP_Net_t *NetData = (SBN_TCP_Net_t *)Net->ModulePvt;
    SBN_PeerIdx_t PeerIdx = 0;

    OS_time_t LocalTime;
    OS_GetLocalTime(&LocalTime);

    OS_SockFileDes_t ClientFd = 0;
    OS_SockAddr_t Addr;
    /* NOTE: OSAL currently has a bug that causes OS_SocketAccept to fail, see ticket #349. */
    while((Status = OS_SocketAccept(NetData->Socket, &ClientFd, &Addr, 0)) == OS_SUCCESS)
    {
        NewConn(NetData, ClientFd);
    }/* end while */

    if (Status != OS_ERROR_TIMEOUT)
    {
        CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_ERROR, "CPU accept error");
    }/* end if */

    /**
     * For peers I connect out to, and which are not currently connected,
     * try connecting now.
     */
    for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
    {
        SBN_PeerInterface_t *Peer = &Net->Peers[PeerIdx];
        SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;

        if(PeerData->ConnectOut && !Peer->Connected)
        {
            /* TODO: make a #define */
            if(LocalTime.seconds > PeerData->LastConnectTry.seconds + 5)
            {
                CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_INFORMATION,
                    "connecting to peer (PeerData=0x%lx, ProcessorID=%d)",
                    (unsigned long int)PeerData, Peer->ProcessorID);

                PeerData->LastConnectTry.seconds = LocalTime.seconds;

                OS_SocketID_t Socket = 0;

                if(OS_SocketOpen(&Socket, OS_SocketDomain_INET, OS_SocketType_STREAM) != OS_SUCCESS)
                {
                    CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_ERROR, "unable to create socket");
                    return;
                }/* end if */

                /* TODO: Make timeout configurable */
                if(OS_SocketConnect(Socket, &PeerData->Addr, 100) != OS_SUCCESS)
                {
                    CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_ERROR,
                        "unable to connect to peer (PeerData=0x%lx)",
                        (unsigned long int)PeerData);

                    OS_close(Socket);

                    return;
                }/* end if */

                CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_INFORMATION, "CPU %d connected", Peer->ProcessorID);

                SBN_TCP_Conn_t *Conn = NewConn(NetData, Socket);
                if(Conn)
                {
                    Conn->PeerInterface = Peer;
                    PeerData->Conn = Conn;

                    SBN_Connected(Peer);
                }/* end if */
            }/* end if */
        }/* end if */
    }/* end for */
}/* end CheckNet() */

static SBN_MsgSz_t Send(SBN_PeerInterface_t *Peer, SBN_MsgType_t MsgType, SBN_MsgSz_t MsgSz, void *Msg)
{
    SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;
    SBN_NetInterface_t *Net = Peer->Net;
    SBN_TCP_Net_t *NetData = (SBN_TCP_Net_t *)Net->ModulePvt;

    if (PeerData->Conn == NULL)
    {
        /* fail silently as the peer is not connected (yet) */
        return 0;
    }/* end if */

    SBN_PackMsg(&SendBufs[NetData->BufNum], MsgSz, MsgType, CFE_PSP_GetProcessorId(), Msg);
    SBN_MsgSz_t sent_size = OS_write(PeerData->Conn->Socket, &SendBufs[NetData->BufNum], MsgSz + SBN_PACKED_HDR_SZ);
    if(sent_size < MsgSz + SBN_PACKED_HDR_SZ)
    {
        CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_INFORMATION,
            "CPU %d failed to write, disconnected", Peer->ProcessorID);

        Disconnected(Peer);
        return 0;
    }/* end if */

    return sent_size;
}/* end Send() */

static SBN_Status_t PollPeer(SBN_PeerInterface_t *Peer)
{
    CheckNet(Peer->Net);

    if(!Peer->Connected)
    {
        return SBN_SUCCESS;
    }/* end if */

    OS_time_t CurrentTime;
    OS_GetLocalTime(&CurrentTime);

    if(SBN_TCP_PEER_HEARTBEAT > 0 && CurrentTime.seconds - Peer->LastSend.seconds > SBN_TCP_PEER_HEARTBEAT)
    {
        Send(Peer, SBN_TCP_HEARTBEAT_MSG, 0, NULL);
    }/* end if */

    if(SBN_TCP_PEER_TIMEOUT > 0 && CurrentTime.seconds - Peer->LastRecv.seconds > SBN_TCP_PEER_TIMEOUT)
    {
        CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_INFORMATION, "CPU %d timeout, disconnected", Peer->ProcessorID);

        Disconnected(Peer);
    }/* end if */

    return SBN_SUCCESS;
}/* end PollPeer() */

static SBN_Status_t Recv(SBN_NetInterface_t *Net,
    SBN_MsgType_t *MsgTypePtr, SBN_MsgSz_t *MsgSzPtr,
    CFE_ProcessorID_t *ProcessorIDPtr, void *MsgBuf)
{
    OS_FdSet FdSet;
    OS_SelectTimeout_t timeout = 0;
    int ConnID = 0;

    SBN_TCP_Net_t *NetData = (SBN_TCP_Net_t *)Net->ModulePvt;

    if (Net->TaskFlags & SBN_TASK_RECV)
    {
        timeout = 1000;
    }/* end if */

    OS_SelectFdZero(&FdSet);

    for(ConnID = 0; ConnID < SBN_MAX_PEER_CNT; ConnID++)
    {
        if (NetData->Conns[ConnID].InUse)
        {
            OS_SelectFdAdd(&FdSet, NetData->Conns[ConnID].Socket);
        }/* end if */
    }/* end for */

    if(OS_SelectMultiple(&FdSet, NULL, timeout) != OS_SUCCESS)
    {
        return SBN_IF_EMPTY;
    }/* end if */

    for(ConnID = 0; ConnID < SBN_MAX_PEER_CNT; ConnID++)
    {
        SBN_TCP_Conn_t *Conn = &NetData->Conns[ConnID];

        if (!Conn->InUse)
        {
            continue;
        }/* end if */

        if(OS_SelectFdIsSet(&FdSet, Conn->Socket))
        {
            ssize_t Received = 0;

            int ToRead = 0;

            if(!Conn->ReceivingBody)
            {
                /* recv the header first */
                ToRead = SBN_PACKED_HDR_SZ - Conn->RecvSz;

                Received = OS_read(Conn->Socket,
                    (char *)&RecvBufs[Conn->BufNum] + Conn->RecvSz,
                    ToRead);

                if(Received <= 0)
                {
                    CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_INFORMATION,
                        "Connection %d head recv failed, disconnected",ConnID);

                    OS_close(Conn->Socket);

                    Disconnected(Conn->PeerInterface);

                    return SBN_IF_EMPTY;
                }/* end if */

                Conn->RecvSz += Received;

                if(Received >= ToRead)
                {
                    Conn->ReceivingBody = TRUE; /* and continue on to recv body */
                }
                else
                {
                    return SBN_IF_EMPTY; /* wait for the complete header */
                }/* end if */
            }/* end if */

            /* only get here if we're recv'd the header and ready for the body */

            ToRead = CFE_MAKE_BIG16(
                    *((SBN_MsgSz_t *)&RecvBufs[Conn->BufNum])
                ) + SBN_PACKED_HDR_SZ - Conn->RecvSz;
            if(ToRead)
            {
                Received = OS_read(Conn->Socket,
                    (char *)&RecvBufs[Conn->BufNum] + Conn->RecvSz,
                    ToRead);

                if(Received <= 0)
                {
                    CFE_ProcessorID_t ProcessorID = -1;
                    if(Conn->PeerInterface != NULL)
                    {
                        ProcessorID = Conn->PeerInterface->ProcessorID;
                    }/* end if */

                    CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_INFORMATION,
                        "CPUID %d body recv failed, disconnected", ProcessorID);

                    return SBN_ERROR;
                }/* end if */

                Conn->RecvSz += Received;

                if(Received < ToRead)
                {
                    return SBN_IF_EMPTY; /* wait for the complete body */
                }/* end if */
            }/* end if */

            /* we have the complete body, decode! */
            if(SBN_UnpackMsg(&RecvBufs[Conn->BufNum], MsgSzPtr, MsgTypePtr,
                ProcessorIDPtr, MsgBuf) == FALSE)
            {
                return SBN_ERROR;
            }/* end if */

            if(!Conn->PeerInterface)
            {
                /* New peer, link it to the connection */
                int PeerInterfaceID = 0;

                for (PeerInterfaceID = 0; PeerInterfaceID < Net->PeerCnt; PeerInterfaceID++)
                {
                    SBN_PeerInterface_t *PeerInterface = &Net->Peers[PeerInterfaceID];

                    if(PeerInterface->ProcessorID == *ProcessorIDPtr)
                    {
                        SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)PeerInterface->ModulePvt;

                        PeerData->Conn = Conn;

                        Conn->PeerInterface = PeerInterface;

                        SBN_Connected(PeerInterface);

                        break;
                    }/* end if */
                }/* end for */
            }/* end if */

            Conn->ReceivingBody = FALSE;
            Conn->RecvSz = 0;
        }/* end if */
    }/* end for */

    return SBN_SUCCESS;
}/* end Recv() */

static SBN_Status_t UnloadPeer(SBN_PeerInterface_t *Peer)
{
    Disconnected(Peer);

    return SBN_SUCCESS;
}/* end UnloadPeer() */

static SBN_Status_t UnloadNet(SBN_NetInterface_t *Net)
{
    SBN_TCP_Net_t *NetData = (SBN_TCP_Net_t *)Net->ModulePvt;

    if(NetData->Socket)
    {
        close(NetData->Socket);
    }/* end if */

    SBN_PeerIdx_t PeerIdx = 0;
    for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
    {
        UnloadPeer(&Net->Peers[PeerIdx]);
    }/* end if */

    return SBN_SUCCESS;
}/* end UnloadNet() */

SBN_IfOps_t SBN_TCP_Ops =
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
