#include "sbn_tbl.h"
//#include "cfe_tbl_filedef.h"

SBN_RemapTbl_t SBN_RemapTbl =
{ 
    SBN_REMAP_DEFAULT_IGNORE, /* Remap Default */
    0, /* number of entries, initialized at validation time */
    {  /* remap table */
        /* {CPU_ID, from, to} and if to is 0x0000, filter rather than remap */
        {1, 0x0601, 0x0601},
        {1, 0x0602, 0x0602},
        {1, 0x0603, 0x0603},
        {1, 0x0604, 0x0604},
        {1, 0x0606, 0x0606},
        {1, 0x0817, 0x0817},
        {2, 0x0601, 0x0601},
        {2, 0x0602, 0x0602},
        {2, 0x0603, 0x0603},
        {2, 0x0604, 0x0604},
        {2, 0x0606, 0x0606},
        {2, 0x0817, 0x0817},
        {3, 0x0601, 0x0601},
        {3, 0x0602, 0x0602},
        {3, 0x0603, 0x0603},
        {3, 0x0604, 0x0604},
        {3, 0x0606, 0x0606},
        {3, 0x0817, 0x0817}

    }
};/* end SBN_RemapTbl */

//CFE_TBL_FILEDEF(SBN_RemapTbl, SBN.SBN_RemapTbl, SBN Remap Table, sbn_remap_tbl.tbl)
