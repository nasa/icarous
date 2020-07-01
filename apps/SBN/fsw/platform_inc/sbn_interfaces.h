#ifndef _sbn_interfaces_h_
#define _sbn_interfaces_h_

#include "cfe.h"
#include "sbn_types.h"
#include "sbn_msg.h"

/**
 * @note The packed size is likely smaller than an in-memory's struct
 * size, as the compiler will align objects.
 * SBN headers are MsgSz + MsgType + ProcessorID
 * SBN subscription messages are MsgID + QoS
 */

#define SBN_PACKED_HDR_SZ (sizeof(SBN_MsgSz_t) + sizeof(SBN_MsgType_t) + sizeof(CFE_ProcessorID_t))
#define SBN_PACKED_SUB_SZ (SBN_PACKED_HDR_SZ + sizeof(SBN_SubCnt_t) + sizeof(CFE_SB_MsgId_t) * SBN_MAX_SUBS_PER_PEER)
#define SBN_MAX_PACKED_MSG_SZ (SBN_PACKED_HDR_SZ + CFE_SB_MAX_SB_MSG_SIZE)

/**
 * @brief Used by modules to pack messages to send.
 *
 * @param SBNMsgBuf[out] The buffer pointer to receive the packed message.
 *                       Should be MsgSz + SBN_PACKED_HDR_SZ bytes or larger.
 * @param MsgSz[in] The size of the Msg parameter.
 * @param MsgType[in] The type of the Msg (app, sub/unsub, heartbeat, announce).
 * @param ProcessorID[in] The Processor ID of the sender (should be CFE_CPU_ID)
 * @param Msg[in] The SBN message payload (CCSDS message, sub/unsub)
 *
 * @sa SBN_UnpackMsg
 */
void SBN_PackMsg(void *SBNMsgBuf, SBN_MsgSz_t MsgSz, SBN_MsgType_t MsgType, CFE_ProcessorID_t ProcessorID, void *Msg);

/**
 * @brief Used by modules to unpack messages received.
 *
 * @param SBNMsgBuf[in] The buffer pointer containing the SBN message.
 * @param MsgSzPtr[out] The size of the Msg parameter.
 * @param MsgTypePtr[out] The type of the Msg (app, sub/unsub, heartbeat, announce).
 * @param ProcessorID[out] The Processor ID of the sender (should be CFE_CPU_ID)
 * @param Msg[out] The SBN message payload (CCSDS message, sub/unsub, ensure it is at least CFE_SB_MAX_SB_MSG_SIZE)
 * @return TRUE if we were unable to unpack/verify the message.
 *
 * @sa SBN_PackMsg
 */
bool SBN_UnpackMsg(void *SBNBuf, SBN_MsgSz_t *MsgSzPtr, SBN_MsgType_t *MsgTypePtr,
    CFE_ProcessorID_t *ProcessorIDPtr, void *Msg);

/**
 * Filters modify messages in place, doing such things as byte swapping, packing/unpacking, etc.
 *
 * @return SBN_SUCCESS if processing nominal, SBN_IF_EMPTY if the message should not be transmitted, SBN_ERROR for 
 * other error conditions.
 */
typedef struct
{
    CFE_ProcessorID_t MyProcessorID;
    CFE_SpacecraftID_t MySpacecraftID;

    CFE_ProcessorID_t PeerProcessorID;
    CFE_SpacecraftID_t PeerSpacecraftID;
} SBN_Filter_Ctx_t;

typedef struct
{
    /**
     * Initializes the filter module.
     *
     * @param FilterVersion[in] The version # of the filter API.
     * @param BaseEID[in] The start of the Event ID's for this module.
     *
     * @return CFE_SUCCESS on successful initialization, otherwise error specific to failure.
     */
    CFE_Status_t (*InitModule)(int FilterVersion, CFE_EVS_EventID_t BaseEID);

    /**
     * Interface is called to apply a filter algorithm on an SB (CCSDS) message
     * header and body.
     *
     * @param MsgBuf[inout] The message buffer to alter in-place.
     * @param Context[in] The context information for this message (particularly peer info.)
     *
     * @return SBN_SUCCESS when the filter feels it processed the message successfully
     *         (this doesn't necessarily mean the message was altered.)
     *         SBN_IF_EMPTY if the filter believes the message should not be relayed.
     *         SBN_ERROR for all other error conditions.
     */
    SBN_Status_t (*FilterRecv)(void *MsgBuf, SBN_Filter_Ctx_t *Context);

    /**
     * Interface is called to apply a filter algorithm on an SB (CCSDS) message
     * header and body.
     *
     * @param MsgBuf[inout] The message buffer to alter in-place.
     * @param Context[in] The context information for this message (particularly peer info.)
     *
     * @return SBN_SUCCESS when the filter feels it processed the message successfully
     *         (this doesn't necessarily mean the message was altered.)
     *         SBN_IF_EMPTY if the filter believes the message should not be relayed.
     *         SBN_ERROR for all other error conditions.
     */
    SBN_Status_t (*FilterSend)(void *MsgBuf, SBN_Filter_Ctx_t *Context);

    /**
     * Some filter interfaces may alter the message ID of the messages it processes.
     * SBN needs to know this when it relays subscription information to peers.
     *
     * @param FromToMidPtr[inout] A pointer to the message id to be altered, and the resulting
     *        message id when it has been altered.
     * @param Context[in] The context information for this request (particularly peer info.)
     *
     * @return SBN_SUCCESS when the function feels it processed the message remapping successfully
     *         (this doesn't necessarily mean the id was altered.)
     *         SBN_IF_EMPTY if the filter believes the subscription should not be relayed.
     *         SBN_ERROR for all other error conditions.
     */
    SBN_Status_t (*RemapMID)(CFE_SB_MsgId_t *FromToMidPtr, SBN_Filter_Ctx_t *Context);
} SBN_FilterInterface_t;

typedef struct SBN_IfOps_s SBN_IfOps_t;
typedef struct SBN_NetInterface_s SBN_NetInterface_t;

typedef struct {
    /** @brief The processor ID of this peer (MUST match the ProcessorID.) */
    CFE_ProcessorID_t ProcessorID;

    /** @brief The Spacecraft ID of this peer (MUST match the SpacecraftID.) */
    CFE_SpacecraftID_t SpacecraftID;

    /** @brief A convenience pointer to the net that this peer belongs to. */
    SBN_NetInterface_t *Net;

    SBN_Task_Flag_t TaskFlags;

    /**
     * @brief The ID of the task created to pend on the pipe and send messages
     * to the net as soon as they are read. 0 if there is no send task.
     */
    OS_TaskID_t SendTaskID;

    /**
     * @brief The ID of the task created to pend on the net and send messages
     * to the software bus as soon as they are read. 0 if there is no recv task.
     */
    OS_TaskID_t RecvTaskID; /* for mesh nets */

    /** @brief The pipe ID used to read messages destined for the peer. */
    CFE_SB_PipeId_t Pipe;

    /**
     * @brief A local table of subscriptions the peer has requested.
     * Includes one extra entry for a null termination.
     */
    SBN_Subs_t Subs[SBN_MAX_SUBS_PER_PEER + 1];

    /**
     * @brief Filters alter message headers/bodies before sending to a peer or after
     *        receiving from the peer.
     */
    SBN_FilterInterface_t *Filters[SBN_MAX_FILTERS];
    SBN_ModuleIdx_t FilterCnt;

    OS_time_t LastSend, LastRecv;
    SBN_HKTlm_t SendCnt, RecvCnt, SendErrCnt, RecvErrCnt, SubCnt;

    bool Connected;

    /** @brief generic blob of bytes for the module-specific data. */
    uint8 ModulePvt[128];
} SBN_PeerInterface_t;

/**
 * @brief When a module detects a (re)connection, a full subscription update
 * should be sent.
 *
 * @param Peer[in] The peer to which to send the subs.
 */
void SBN_SendLocalSubsToPeer(SBN_PeerInterface_t *Peer);

/**
 * @brief For a given network and processor ID, get the peer interface.
 *
 * @param Net[in] The network to check.
 * @param ProcessorID[in] The processor of the peer.
 *
 * @return A pointer to the peer interface structure.
 */
SBN_PeerInterface_t *SBN_GetPeer(SBN_NetInterface_t *Net, CFE_ProcessorID_t ProcessorID);

struct SBN_NetInterface_s {
    bool Configured;

    SBN_ModuleIdx_t ProtocolIdx;

    SBN_Task_Flag_t TaskFlags;

    /* For some network topologies, this application only needs one connection
     * to communicate to peers. These tasks are used for those networks. ID's
     * are 0 if there is no task.
     */
    OS_TaskID_t SendTaskID;
    OS_MutexID_t SendMutex;

    OS_TaskID_t RecvTaskID;

    SBN_IfOps_t *IfOps; /* convenience */

    SBN_PeerIdx_t PeerCnt;

    SBN_PeerInterface_t Peers[SBN_MAX_PEER_CNT];

    /**
     * @brief Filters alter message headers/bodies before sending to a peer or after
     *        receiving from the peer.
     */
    SBN_FilterInterface_t *Filters[SBN_MAX_FILTERS];
    SBN_ModuleIdx_t FilterCnt;

    /** @brief generic blob of bytes, module-specific */
    uint8  ModulePvt[128];
};

/**
 * This structure contains function pointers to interface-specific versions
 * of the key SBN functions.  Every interface module must have an equivalent
 * structure that points to the approprate functions for that interface.
 */
struct SBN_IfOps_s {
    /**
     * Initializes the protocol module.
     *
     * @param FilterVersion[in] The version # of the protocol API.
     * @param BaseEID[in] The start of the Event ID's for this module.
     *
     * @return CFE_SUCCESS on successful initialization, otherwise error specific to failure.
     */
    CFE_Status_t (*InitModule)(int ProtocolVersion, CFE_EVS_EventID_t BaseEID);

    /** TODO: Document */
    SBN_Status_t (*LoadNet)(SBN_NetInterface_t *Net, const char *Address);
    /** TODO: Document */
    SBN_Status_t (*LoadPeer)(SBN_PeerInterface_t *Peer, const char *Address);

    /**
     * Initializes the host interface.
     *
     * @param Net[in,out] Struct pointer describing a single interface
     * @return SBN_SUCCESS on successful initialization
     *         SBN_ERROR otherwise
     */
    SBN_Status_t (*InitNet)(SBN_NetInterface_t *Host);

    /**
     * Initializes the peer interface.
     *
     * @param Peer[in,out] The peer interface to initialize
     * @return SBN_SUCCESS on successful initialization
     *         SBN_ERROR otherwise
     */
    SBN_Status_t (*InitPeer)(SBN_PeerInterface_t *Peer);

    /**
     * SBN will poll any peer that does not have any messages to be sent
     * after a timeout period. This is for (re)establishing connections
     * and handshaking subscriptions.
     *
     * @param Peer[in] The peer to poll.
     *
     * @return SBN_SUCCESS on successful polling, SBN_ERROR otherwise.
     */
    SBN_Status_t (*PollPeer)(SBN_PeerInterface_t *Peer);

    /**
     * Sends a message to a peer over the specified interface.
     * Both protocol and data message buffers are included in the parameters,
     * but only one is used at a time.  The data message buffer is used for
     * un/subscriptions and app messages.  The protocol message buffer is used
     * for announce and heartbeat messages/acks.
     *
     * @param Net[in] Interface data for the network where this peer lives.
     * @param Peer[in] Interface data describing the intended peer recipient.
     * @param MsgType[in] The SBN message type.
     * @param MsgSz[in] The size of the SBN message payload.
     * @param Payload[in] The SBN message payload.
     *
     * @return Number of bytes sent on success, -1 on error.
     */
    SBN_MsgSz_t (*Send)(SBN_PeerInterface_t *Peer, SBN_MsgType_t MsgType,
        SBN_MsgSz_t MsgSz, void *Payload);

    /**
     * Receives an individual message from the specified peer. Note, only
     * define this or the RecvFromNet method, not both!
     *
     * @param Net[in] Interface data for the network where this peer lives.
     * @param Peer[in] Interface data describing the intended peer recipient.
     * @param MsgTypePtr[out] SBN message type received.
     * @param MsgSzPtr[out] Payload size received.
     * @param ProcessorIDPtr[out] ProcessorID of the sender.
     * @param PayloadBuffer[out] Payload buffer
     *                      (pass in a buffer of CFE_SB_MAX_SB_MSG_SIZE)
     *
     * @return SBN_SUCCESS on success, SBN_ERROR on failure
     */
    SBN_Status_t (*RecvFromPeer)(SBN_NetInterface_t *Net, SBN_PeerInterface_t *Peer, SBN_MsgType_t *MsgTypePtr,
        SBN_MsgSz_t *MsgSzPtr, CFE_ProcessorID_t *ProcessorIDPtr, void *PayloadBuffer);

    /**
     * Receives an individual message from the network.
     *
     * @param Net[in] Interface data for the network where this peer lives.
     * @param MsgTypePtr[out] SBN message type received.
     * @param MsgSzPtr[out] Payload size received.
     * @param ProcessorIDPtr[out] ProcessorID of the sender.
     * @param PayloadBuffer[out] Payload buffer
     *                      (pass in a buffer of CFE_SB_MAX_SB_MSG_SIZE)
     *
     * @return SBN_SUCCESS on success, SBN_ERROR on failure
     */
    SBN_Status_t (*RecvFromNet)(SBN_NetInterface_t *Net, SBN_MsgType_t *MsgTypePtr,
        SBN_MsgSz_t *MsgSzPtr, CFE_ProcessorID_t *ProcessorIDPtr,
        void *PayloadBuffer);

    /**
     * Unload a network. This will unload all associated peers as well.
     *
     * @param Net[in] Network to unload.
     *
     * @return  SBN_SUCCESS when the net is unloaded.
     *          SBN_ERROR if the net cannot be unloaded.
     *          SBN_NOT_IMPLEMENTED if the module does not implement this
     *          function.
     *
     * @sa LoadNet, LoadPeer, UnloadPeer
     */
    SBN_Status_t (*UnloadNet)(SBN_NetInterface_t *Net);

    /**
     * Unload a peer.
     *
     * @param Peer[in] Peer to unload.
     *
     * @return  SBN_SUCCESS when the peer is unloaded.
     *          SBN_ERROR if the peer cannot be unloaded.
     *          SBN_NOT_IMPLEMENTED if the module does not implement this
     *          function.
     *
     * @sa LoadNet, LoadPeer, UnloadNet
     */
    SBN_Status_t (*UnloadPeer)(SBN_PeerInterface_t *Peer);
};

/**
 * @brief Called by backend modules to signal that the connection has been
 * established and that the initial handshake should ensue.
 */
SBN_Status_t SBN_Connected(SBN_PeerInterface_t *Peer);

/**
 * @brief Called by backend modules to signal that the connection has been
 * lost.
 */
SBN_Status_t SBN_Disconnected(SBN_PeerInterface_t *Peer);

/**
 * @brief Used by modules to send protocol-specific messages
 * (particularly UDP which needs to send announcement/heartbeat msgs.)
 */
SBN_MsgSz_t SBN_SendNetMsg(SBN_MsgType_t MsgType, SBN_MsgSz_t MsgSz, void *Msg, SBN_PeerInterface_t *Peer);

#endif /* _sbn_interfaces_h_ */
