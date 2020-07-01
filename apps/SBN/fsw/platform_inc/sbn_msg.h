#ifndef _sbn_msg_h_
#define _sbn_msg_h_

#include "sbn_msgdefs.h"
#include "sbn_platform_cfg.h"
#include "sbn_types.h"
#include "cfe.h"

#define SBN_CMD_NET_LEN CFE_SB_CMD_HDR_SIZE + sizeof(SBN_NetIdx_t)

#define SBN_CMD_PEER_LEN CFE_SB_CMD_HDR_SIZE + sizeof(SBN_PeerIdx_t)

/** @brief uint8 Enabled, uint8 DefaultFlag */
#define SBN_CMD_REMAPCFG_LEN CFE_SB_CMD_HDR_SIZE + 2

/** @brief ProcessorID, FromMID, ToMID */
#define SBN_CMD_REMAPADD_LEN CFE_SB_CMD_HDR_SIZE + sizeof(CFE_ProcessorID_t) + sizeof(CFE_SB_MsgId_t) * 2

/** @brief ProcessorID, FromMID */
#define SBN_CMD_REMAPDEL_LEN CFE_SB_CMD_HDR_SIZE + sizeof(CFE_ProcessorID_t) + sizeof(CFE_SB_MsgId_t)

/** @brief CC, CmdCnt, CmdErrCnt, SubCnt, NetCnt */
#define SBN_HK_LEN (CFE_SB_TLM_HDR_SIZE + sizeof(uint8) + (sizeof(SBN_HKTlm_t) * 4))

/** @brief CC, SBN_SubCnt_t SubCnt, CFE_SB_MsgId_t Subs[SBN_MAX_SUBS_PER_PEER] */
#define SBN_HKMYSUBS_LEN (CFE_SB_TLM_HDR_SIZE + sizeof(uint8) + sizeof(SBN_SubCnt_t) + SBN_MAX_SUBS_PER_PEER * sizeof(CFE_SB_MsgId_t))

/** @brief CC, NetIdx, PeerIdx, SubCnt, Subs[SBN_MAX_SUBS_PER_PEER] */
#define SBN_HKPEERSUBS_LEN (CFE_SB_TLM_HDR_SIZE + sizeof(uint8) + sizeof(SBN_NetIdx_t) + sizeof(SBN_PeerIdx_t) + sizeof(SBN_SubCnt_t) + SBN_MAX_SUBS_PER_PEER * sizeof(CFE_SB_MsgId_t))

/** @brief CC, SubCnt, ProcessorID, LastSend, LastRecv, SendCnt, RecvCnt, SendErrCnt, RecvErrCnt */
#define SBN_HKPEER_LEN (CFE_SB_TLM_HDR_SIZE + sizeof(uint8) + sizeof(SBN_SubCnt_t) + sizeof(CFE_ProcessorID_t) + sizeof(OS_time_t) * 2 + sizeof(SBN_HKTlm_t) * 4)

/** @brief CC, ProtocolID, PeerCnt */
#define SBN_HKNET_LEN (CFE_SB_TLM_HDR_SIZE + sizeof(uint8) + sizeof(SBN_ModuleIdx_t) + sizeof(SBN_PeerIdx_t))

/**
 * @brief Module status response packet structure
 */
typedef struct {
    /**
     * This is a struct that will be sent (as-is) in a response to a HK command.
     */
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    /** @brief The Protocol ID being queried. */
    SBN_ModuleIdx_t  ProtocolIdx;
    /** @brief The module status as returned by the module. */
    uint8   ModuleStatus[SBN_MOD_STATUS_MSG_SZ];
} SBN_ModuleStatusPacket_t;

#endif /* _sbn_msg_h_ */
