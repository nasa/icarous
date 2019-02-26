
#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */
#include "apInterface_table.h"

ApInterfaceTable_t ApInterface_TblStruct = {
	.PortType = SOCKET,
	.BaudRate = 0,
	.Portin = 14551,
	.Portout = 0,
	.Address = "127.0.0.1",
};



/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(ApInterface_TblStruct, APINTERFACE.APInterfaceTable, Interface parameters, apIntf_tbl.tbl )
