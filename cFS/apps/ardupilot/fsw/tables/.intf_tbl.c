
#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */
#include "ardupilot_table.h"

#ifdef SITL

ArdupilotTable_t Ardupilot_TblStruct = {
	.PortType = SOCKET,
	.BaudRate = 0,
	.Portin = 14551,
	.Portout = 0,
	.Address = "127.0.0.1",
};

#else
ArdupilotTable_t Ardupilot_TblStruct = {
    .PortType = SERIAL,       // apPortType
    .BaudRate = 57600,          // baudrate
    .Portin = 0,              // apPortin
    .Portout = 0,              // apPortout
    .Address = "/dev/ttyACM0", // ap address
};
#endif


/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(Ardupilot_TblStruct, ARDUPILOT.InterfaceTable, Interface parameters, intf_tbl.tbl )
