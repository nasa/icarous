/*
 * icarous_tbl.c
 *
 */

#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */
#include "icarous_table.h"

icarous_table_t icarousTblStruct = {
		5,  // ICAROUS heartbeat;
		5,  // Takeoff alt
		1,  // Allow yaw
		30, // Max ceiling

		1.0, // Hthreshold
		1.0, // Vthreshold
		2.0, // hstepback
		2.0, // vstepback

		0,   // cheapsearch
		0.5, // gridsize
		1,   // buffer
		1,   // lookahead
		1.0, // prox factor
		1.0,   // resolution speed

		0.6,   // xtrk gain
		500.0, // xtrk dev
		1.0, // CaptureH
		1.0, // CaptureV
		0,     // Wait

		270,   //track heading
		3,     // track distance H
		0,     // track distance V

		1,  //Cheap DAA
		1,  // goto next WP
		3,  // conflict hold in seconds
		5,  // cylinder radius
		10, // cylinder height
		10, // alert time
		15, // early alert time
		20  // daa look ahead
};


/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(icarousTblStruct, ICAROUS.IcarousTable, Icarous parameters, icarous_tbl.tbl )
