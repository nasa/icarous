//
// Created by Swee Balachandran on 11/13/17.
//

#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */

#include "rotorsim_table.h"


RotorsimTable_t RotorsimTblStruct = {
   1,                            // speed
   {37.906021,-77.046822},       // origin
   10,                           // Initial altitude
   2,                            // Total number of waypoints
   {{37.102386, -76.387462, 10},
    {37.102418, -76.386239, 10},
    {0,0,0},
    {0,0,0},
    {0,0,0}}                     // Flight plan
};


/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(RotorsimTblStruct, ROTORSIM.RotorsimTable, Rotorsim parameters, rotorsim_tbl.tbl )
