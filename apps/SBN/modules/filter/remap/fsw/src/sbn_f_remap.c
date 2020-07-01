#include "sbn_interfaces.h"
#include "sbn_remap_tbl.h"
#include "sbn_msgids.h"
#include "cfe.h"
#include "cfe_tbl.h"
#include <string.h> /* memcpy */
#include <stdlib.h> /* qsort */

#include "sbn_f_remap_events.h"

CFE_TBL_Handle_t RemapTblHandle = 0;
OS_MutexID_t RemapMutex = 0;
SBN_RemapTbl_t *RemapTbl = NULL;
int RemapTblCnt = 0;

CFE_EVS_EventID_t SBN_F_REMAP_FIRST_EID;

static int RemapTblVal(void *TblPtr)
{
    SBN_RemapTbl_t *r = (SBN_RemapTbl_t *)TblPtr;
    int i = 0;

    switch(r->RemapDefaultFlag)
    {
        /* all valid values */
        case SBN_REMAP_DEFAULT_IGNORE:
        case SBN_REMAP_DEFAULT_SEND:
            break;
        /* otherwise, unknown! */
        default:
            return -1;
    }/* end switch */

    /* Find the first "empty" entry (with a 0x0000 "from") to determine table
     * size.
     */
    for(i = 0; i < SBN_REMAP_TABLE_SIZE; i++)
    {
        if (r->Entries[i].FromMID == 0x0000)
        {
            break;
        }/* end if */
    }/* end for */

    RemapTblCnt = i;

    return 0;
}/* end RemapTblVal() */

static int RemapTblCompar(const void *a, const void *b)
{
    SBN_RemapTblEntry_t *aEntry = (SBN_RemapTblEntry_t *)a;
    SBN_RemapTblEntry_t *bEntry = (SBN_RemapTblEntry_t *)b;

    if(aEntry->ProcessorID != bEntry->ProcessorID)
    {   
        return aEntry->ProcessorID - bEntry->ProcessorID;
    }
    return aEntry->FromMID - bEntry->FromMID;
}/* end RemapTblCompar() */

static SBN_Status_t LoadRemap(void)
{
    CFE_Status_t Status = CFE_SUCCESS;
    SBN_RemapTbl_t *TblPtr;

    if((Status = CFE_TBL_GetAddress((void **)&TblPtr, RemapTblHandle))
        != CFE_TBL_INFO_UPDATED)
    {
        CFE_EVS_SendEvent(SBN_F_REMAP_TBL_EID, CFE_EVS_ERROR,
            "unable to get conf table address");
        CFE_TBL_Unregister(RemapTblHandle);
        return SBN_ERROR;
    }/* end if */

    /* sort the entries on ProcessorID and from MID */
    /* note: qsort is recursive, so it will use some stack space
     * (O[N log N] * <some small amount of stack>). If this is a concern,
     * consider using a non-recursive (insertion, bubble, etc) sort algorithm.
     */

    qsort(RemapTbl->Entries, RemapTblCnt, sizeof(SBN_RemapTblEntry_t), RemapTblCompar);

    CFE_TBL_Modified(RemapTblHandle);

    RemapTbl = TblPtr;

    return SBN_SUCCESS;
}/* end LoadRemap() */

static SBN_Status_t LoadRemapTbl(void)
{
    if(CFE_TBL_Register(&RemapTblHandle, "SBN_RemapTbl", sizeof(SBN_RemapTbl_t),
        CFE_TBL_OPT_DEFAULT, &RemapTblVal) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_F_REMAP_TBL_EID, CFE_EVS_ERROR,
            "unable to register remap tbl handle");
        return SBN_ERROR;
    }/* end if */

    if(CFE_TBL_Load(RemapTblHandle, CFE_TBL_SRC_FILE, SBN_REMAP_TBL_FILENAME) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_F_REMAP_TBL_EID, CFE_EVS_ERROR,
            "unable to load remap tbl %s", SBN_REMAP_TBL_FILENAME);
        CFE_TBL_Unregister(RemapTblHandle);
        return SBN_ERROR;
    }/* end if */

    if(CFE_TBL_Manage(RemapTblHandle) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_F_REMAP_TBL_EID, CFE_EVS_ERROR,
            "unable to manage remap tbl");
        CFE_TBL_Unregister(RemapTblHandle);
        return SBN_ERROR;
    }/* end if */

    if(CFE_TBL_NotifyByMessage(RemapTblHandle, SBN_CMD_MID, SBN_TBL_CC, 1) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_F_REMAP_TBL_EID, CFE_EVS_ERROR,
            "unable to set notifybymessage for remap tbl");
        CFE_TBL_Unregister(RemapTblHandle);
        return SBN_ERROR;
    }/* end if */

    return LoadRemap();
}/* end LoadRemapTbl() */

#if 0
static SBN_Status_t UnloadRemap(void)
{
    RemapTbl = NULL;

    CFE_Status_t Status;

    if((Status = CFE_TBL_ReleaseAddress(RemapTblHandle)) != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_F_REMAP_TBL_EID, CFE_EVS_ERROR,
            "unable to release address for remap tbl");
        return SBN_ERROR;
    }/* end if */

    return SBN_SUCCESS;
}/* end UnloadRemap() */

static SBN_Status_t ReloadRemapTbl(void)
{
    SBN_Status_t Status;

    /* releases the table address */
    if((Status = UnloadRemap()) != SBN_SUCCESS)
    {
        return Status;
    }/* end if */

    if(CFE_TBL_Update(RemapTblHandle) != CFE_SUCCESS)
    {
        return SBN_ERROR;
    }/* end if */

    /* gets the new address and loads config */
    return LoadRemap();
}/* end ReloadRemapTbl() */
#endif /* ifdef out unused fn */

/* finds the entry or the one that would immediately follow it */
static int BinarySearch(void *Entries, void *SearchEntry,
    size_t EntryCnt, size_t EntrySz,
    int (*EntryCompare)(const void *, const void *))
{
    int start, end, midpoint, found;

    for(start = 0, end = EntryCnt - 1, found = 0;
        found == 0 && start <= end;
        )
    {
        midpoint = (end + start) / 2;
        int c = EntryCompare(SearchEntry, (uint8 *)Entries + EntrySz * midpoint);
        if (c == 0)
        {
            return midpoint;
        }
        else if (c > 0)
        {
            start = midpoint + 1;
        }
        else
        {   
           end = midpoint - 1;
        }/* end if */
    }/* end while */

    if(found == 0)
    {
        return EntryCnt;
    }

    return midpoint;
}/* end BinarySearch() */

static int RemapTblSearch(uint32 ProcessorID, CFE_SB_MsgId_t MID)
{
    SBN_RemapTblEntry_t Entry = {ProcessorID, MID, 0x0000};
    return BinarySearch(RemapTbl->Entries, &Entry,
        RemapTbl->EntryCnt,
        sizeof(SBN_RemapTblEntry_t),
        RemapTblCompar);
}/* end RemapTblSearch() */

static SBN_Status_t Remap(void *msg, SBN_Filter_Ctx_t *Context)
{
    CFE_SB_MsgId_t FromMID = 0x0000, ToMID = 0x0000;
    CCSDS_PriHdr_t *PriHdrPtr = msg;

    FromMID = CCSDS_RD_SID(*PriHdrPtr);

    if(OS_MutSemTake(RemapMutex) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_F_REMAP_EID, CFE_EVS_ERROR, "unable to take mutex");
        return SBN_ERROR;
    }/* end if */

    int i = RemapTblSearch(Context->PeerProcessorID, FromMID);

    if(i < RemapTbl->EntryCnt
        && RemapTbl->Entries[i].ProcessorID == Context->PeerProcessorID
        && RemapTbl->Entries[i].FromMID == FromMID)
    {
        ToMID = RemapTbl->Entries[i].ToMID;
    }
    else
    {
        if(RemapTbl->RemapDefaultFlag == SBN_REMAP_DEFAULT_SEND)
        {
            ToMID = FromMID;
        }/* end if */
    }/* end if */

    if(OS_MutSemGive(RemapMutex) != OS_SUCCESS)
    {   
        CFE_EVS_SendEvent(SBN_F_REMAP_EID, CFE_EVS_ERROR, "unable to give mutex");
        return SBN_ERROR;
    }/* end if */

    if(ToMID == 0x0000)
    {
        return SBN_IF_EMPTY; /* signal to the core app that this filter recommends not sending this message */
    }/* end if */

    CCSDS_WR_SID(*PriHdrPtr, ToMID);

    return SBN_SUCCESS;
}/* end Remap() */

static SBN_Status_t Remap_MID(CFE_SB_MsgId_t *InOutMsgIdPtr, SBN_Filter_Ctx_t *Context)
{
    int i = 0;

    for(i = 0; i < RemapTbl->EntryCnt; i++)
    {
        if(RemapTbl->Entries[i].ProcessorID == Context->PeerProcessorID
            && RemapTbl->Entries[i].ToMID == *InOutMsgIdPtr)
        {
            *InOutMsgIdPtr = RemapTbl->Entries[i].FromMID;
            return SBN_SUCCESS;
        }/* end if */
    }/* end for */

    return SBN_SUCCESS;
}/* end Remap_MID() */

static CFE_Status_t Init(int Version, CFE_EVS_EventID_t BaseEID)
{
    OS_Status_t OS_Status;
    CFE_Status_t CFE_Status;

    SBN_F_REMAP_FIRST_EID = BaseEID;

    if(Version != 1) /* TODO: define */
    {
        OS_printf("SBN_F_Remap version mismatch: expected %d, got %d\n", 1, Version);
        return CFE_ES_APP_ERROR;
    }/* end if */

    OS_printf("SBN_F_Remap Lib Initialized.\n");

    OS_Status = OS_MutSemCreate(&RemapMutex, "SBN_F_Remap", 0);
    if(OS_Status != OS_SUCCESS)
    {
        return OS_Status;
    }/* end if */

    CFE_Status = LoadRemapTbl();

    return CFE_Status;
}/* end Init() */

SBN_FilterInterface_t SBN_F_Remap =
{
    Init, Remap, Remap, Remap_MID
};
