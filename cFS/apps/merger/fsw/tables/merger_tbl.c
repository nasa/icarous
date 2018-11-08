//
// Created by Swee Balachandran on 11/13/17.
//

#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */

#include "merger_table.h"


MergerTable_t MergerTblStruct = {
        15,                                         // max vehicle speed
        1,                                          // min vehicle speed
        40,                                         // corridor width
        100,                                         // maneuvering zone
        300,                                        // coordination zone
        30,                                         // min separation distance
        15,                                          // min separation time
        30,                                         // max turn radius
        {{38.909654,-77.043453,10},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}   // intersection location
};


/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(MergerTblStruct, MERGER.MergerTable, Merger parameters, merger_tbl.tbl )
