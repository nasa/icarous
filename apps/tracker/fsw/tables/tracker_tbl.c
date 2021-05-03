//
// Created by Swee Balachandran on 4/30/2021
//

#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */

#include "tracker_tbl.h"

trackerTable_t tracker_TblStruct = {
     .modelUncertaintyP = {100,100,100,0,0,0};
     .modelUncertaintyV = {25,25,25,0,0,0};
     .pThreshold        = 16.27;
     .vThreshold        = 13.82;
};


/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(tracker_TblStruct, TRACKER.trackerTable, tracker parameters, tracker_tbl.tbl )
