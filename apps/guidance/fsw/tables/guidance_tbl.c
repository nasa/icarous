//
// Created by Swee Balachandran on 4/17/2019
//

#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */

#include "guidance_tbl.h"

guidanceTable_t guidance_TblStruct = {
    .defaultWpSpeed = 1.0,
    .captureRadiusScaling = 2,
    .climbFpAngle = 45,
    .climbAngleVRange = 10,
    .climbAngleHRange = 10,
    .climbRateGain = 0.5,
    .maxClimbRate = 5,
    .minClimbRate = -5,
    .yawForward = false
};


/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(guidance_TblStruct, GUIDANCE.GuidanceTable, Guidance parameters, guidance_tbl.tbl )
