/**
 * @file tracking_tbl.c
 * @brief table input values
 */
#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */

#include "tracking_tbl.h"

TrackingTable_t Tracking_TblStruct = {
    true,         // command mode.
    0,            // object id to track
    0.5,          // proportional gain x
    0.5,          // proportional gain y
    0.5,          // proportional gain z
    270,          // heading
    2,            // distH
    2,            // distV
};


/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(Tracking_TblStruct, TRACKING.TrackingTable, Tracking parameters, tracking_tbl.tbl )