#include "sbn_interfaces.h"
#include "sbn_f_ccsds_end_events.h"
#include "cfe.h"

CFE_EVS_EventID_t SBN_F_CCSDS_END_FIRST_EID;

static SBN_Status_t End(void *Msg, SBN_Filter_Ctx_t *Context)
{
    int CCSDSType = CCSDS_RD_TYPE(*((CCSDS_PriHdr_t *)Msg));
    if(CCSDSType == CCSDS_TLM)
    {
        CCSDS_TlmPkt_t *TlmPktPtr = (CCSDS_TlmPkt_t *)Msg;

        uint32 Seconds = CCSDS_RD_SEC_HDR_SEC(TlmPktPtr->Sec);
        Seconds = CFE_MAKE_BIG32(Seconds);
        CCSDS_WR_SEC_HDR_SEC(TlmPktPtr->Sec, Seconds);

        /* SBN sends CCSDS telemetry messages with secondary headers in
         * big-endian order.
         */
        if(CCSDS_TIME_SIZE == 6)
        {
            uint16 SubSeconds = CCSDS_RD_SEC_HDR_SUBSEC(TlmPktPtr->Sec);
            SubSeconds = CFE_MAKE_BIG16(SubSeconds);
            CCSDS_WR_SEC_HDR_SUBSEC(TlmPktPtr->Sec, SubSeconds);
        }
        else
        {
            uint32 SubSeconds = CCSDS_RD_SEC_HDR_SUBSEC(TlmPktPtr->Sec);
            SubSeconds = CFE_MAKE_BIG32(SubSeconds);
            CCSDS_WR_SEC_HDR_SUBSEC(TlmPktPtr->Sec, SubSeconds);
        }/* end if */
    }/* end if */

    return SBN_SUCCESS;
}/* SBN_F_CCSDS_End() */

static CFE_Status_t Init(int Version, CFE_EVS_EventID_t BaseEID)
{
    SBN_F_CCSDS_END_FIRST_EID = BaseEID;

    if(Version != 1) /* TODO: define */
    {
        OS_printf("SBN_F_CCSDS_End version mismatch: expected %d, got %d\n", 1, Version);
        return CFE_ES_APP_ERROR;
    }/* end if */

    OS_printf("SBN_F_CCSDS_End Lib Initialized.\n");
    return CFE_SUCCESS;
}/* end Init() */

SBN_FilterInterface_t SBN_F_CCSDS_End =
{
    Init, End, End, NULL
};
