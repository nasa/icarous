//
// Created by Swee Balachandran on 11/13/17.
//

#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */

#include "merger_table.h"

MergerTable_t MergerTblStruct = {
        .maxVehicleSpeed       = 7,
        .minVehicleSpeed       = 0.5,
        .corridorWidth         = 20,
        .entryRadius           = 50,
        .coordZone             = 80,
        .scheduleZone          = 60,
        .minSeparationDistance = 30,
        .minSeparationTime     = 10,
        .maxVehicleTurnRadius  = 10,
        .IntersectionLocation  = {
                                  //Back 40 merge points
                                  {37.103224,-76.386647,50.000000},//0
                                  {37.103224,-76.384056,50.000000},//1
                                  {37.102416,-76.384056,50.000000},//2
                                  {37.102416,-76.386647,50.000000},//3

                                  // Dryden Street merge points
                                  {37.097996,-76.385895,50.000000},//4
                                  {37.099758,-76.384583,50.000000},//5
                                  {37.099438,-76.383720,50.000000},//6
                                  {37.097588,-76.385033,50.000000} //7
                                 },
        .startIntersection    = 0,
        .missionSpeed         = 3
};




/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(MergerTblStruct, MERGER.MergerTable, Merger parameters, merger_tbl.tbl )
