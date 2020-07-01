#include "sbn_remap_tbl.h"
#include "cfe_tbl_filedef.h"

SBN_RemapTbl_t SBN_RemapTbl =
{ 
    SBN_REMAP_DEFAULT_SEND, /* Remap Default */
    {  /* remap table */
        /* {ProcessorID, from, to} and if to is 0x0000, filter rather than remap */
        {0, 0x0000, 0x0000}
    }
};/* end SBN_RemapTbl */

CFE_TBL_FILEDEF(SBN_RemapTbl, SBN.SBN_RemapTbl, SBN Remap Table, sbn_remap_tbl.tbl)
