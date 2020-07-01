#include "sbn_interfaces.h"
#include "sbn_f_test_events.h"
#include "cfe.h"

CFE_EVS_EventID_t SBN_F_CCSDS_END_FIRST_EID;
static SBN_Status_t In(void *msg, SBN_Filter_Ctx_t *Context)
{
    CCSDS_PriHdr_t *PriHdrPtr = msg;
    OS_printf("msg in StreamId=%d\n", CCSDS_RD_SID(*PriHdrPtr));
    return SBN_SUCCESS;
}/* end In() */

static SBN_Status_t Out(void *msg, SBN_Filter_Ctx_t *Context)
{
    CCSDS_PriHdr_t *PriHdrPtr = msg;
    OS_printf("msg out StreamId=%d\n", CCSDS_RD_SID(*PriHdrPtr));
    return SBN_SUCCESS;
}/* end Out() */

static CFE_Status_t Init(int Version, CFE_EVS_EventID_t BaseEID)
{
    SBN_F_CCSDS_END_FIRST_EID = BaseEID;

    if(Version != 1) /* TODO: define */
    {
        OS_printf("SBN_F_Test version mismatch: expected %d, got %d\n", 1, Version);
        return CFE_ES_APP_ERROR;
    }/* end if */

    OS_printf("SBN_F_Test Lib Initialized.\n");

    return CFE_SUCCESS;
}/* end Init() */

SBN_FilterInterface_t SBN_F_Test =
{
    Init, In, Out, NULL
};
