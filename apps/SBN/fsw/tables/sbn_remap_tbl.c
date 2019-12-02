#include "sbn_tbl.h"
//#include "cfe_tbl_filedef.h"

SBN_RemapTbl_t SBN_RemapTbl =
{ 
    SBN_REMAP_DEFAULT_IGNORE, /* Remap Default */
    0, /* number of entries, initialized at validation time */
    {  /* remap table */
        /* {CPU_ID, from, to} and if to is 0x0000, filter rather than remap */
        {1, 0x18C9, 0x18C9},
        {1, 0x18DF, 0x18DF},
        {1, 0x18D1, 0x18D1},
        {1, 0x18D2, 0x18D2},
        {1, 0x18CA, 0x18CA},
        {1, 0x18DA, 0x18DA},
        {1, 0x0817, 0x0817},
        {1, 0x0140, 0x0140}
    }
};/* end SBN_RemapTbl */

//CFE_TBL_FILEDEF(SBN_RemapTbl, SBN.SBN_RemapTbl, SBN Remap Table, sbn_remap_tbl.tbl)
