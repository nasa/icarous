
#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */
#include "arducopter_table.h"

/*
 * Use this table to specify port settings.
 * Use either a SERIAL port or a SOCKET.
 * BaudRate is only used for SERIAL.
 * Portin and Poutout are only used for SOCKET 
 * Address is IP address if SOCKET or serial port if SERIAL
 */
ArducopterTable_t Arducopter_TblStruct = {
	.PortType = SOCKET,
	.BaudRate = 0,
	.Portin = 14551,
	.Portout = 0,
	.Address = "127.0.0.1",
	.icRcChannel = 7,
	.pwmStart = 1100,
	.pwmReset = 2000
};

/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(Arducopter_TblStruct, ARDUCOPTER.InterfaceTable, Interface parameters, intf_tbl.tbl )
