
#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */
#include "flarm_table.h"

FlarmTable_t Flarm_TblStruct = {
	.PortType = SERIAL,
	.BaudRate = 19200,
	.Portin = 0,
	.Portout = 0,
	.Address = "/dev/flarm"
};



/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(Flarm_TblStruct, FLARM.FlarmTable, Interface parameters, flarm_tbl.tbl )
